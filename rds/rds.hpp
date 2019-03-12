#ifndef _RDS_H
#define _RDS_H
#include <vector>
#include <algorithm>
#include <ctime>
#include <cstdio>
#include <atomic>
#include <chrono>
#include <omp.h>
#include "../verifiers/verifier.hpp"
#include "rds_utils.hpp"

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#include <windows.h>
#include <timeapi.h> //?
#else
#include <unistd.h>
#endif

//std::atomic_uint lb;
static uint lb;
bool should_return = false;
bool should_exit = false;

void print_lb_atomic(int signal)
{
  fprintf(stderr, "\nReceived SIGINT\n");
  fprintf(stderr, "Best lower bound found: %u\n", lb);
  exit(0);
}

void handle_sigalrm(int signal)
{
  should_exit = true;
}

static int nr_calls = 0;

template <typename Verifier> void find_max(std::vector<vertex_set>& c, vertex_set& p, const uint* mu, Verifier *v, Graph* g, std::vector<uint>& res, int level) {
  if(should_return || should_exit)
    return;
  vertex_set& curC = c[level];
//  nr_calls++;

  if(curC.empty())
  {
    if(p.weight <= lb)
      return;
    #pragma omp critical (lbupdate)
    {
      res = p.vertices; //copy
      should_return = true;
      lb = std::max(lb, p.weight);
    }
    return;
  }

  vertex_set& nextC = c[level+1];
  for (uint v_it = 0; v_it < curC.size(); ++v_it) {
    if(curC.weight + p.weight <= lb) // Prune 1
      return;
    if(mu[curC[v_it]] + p.weight <= lb) // Prune 2
      return;

    uint v_weight = g->weight(curC[v_it]);
    curC.weight -= v_weight;

//    NB: exploit that we adding only 1 vertex to p
//    thus verifier can prepare some info using prev calculations
    v->prepare_aux(p, curC[v_it], curC, v_it+1);
    nextC.clear();
    p.add_vertex(curC[v_it], v_weight);
    for(uint u_it = v_it+1; u_it < curC.size(); ++u_it)
      if(v->check(p, curC[v_it], curC[u_it]))
        nextC.add_vertex(curC[u_it], g->weight(curC[u_it]));

    if (nextC.weight + p.weight > lb) // continue if <=
      find_max(c, p, mu, v, g, res, level+1);

    p.pop_vertex(v_weight);
    v->undo_aux(p, curC[v_it], curC, v_it+1);

    if(should_return)
      return;
  }
  return;
}

template <typename Verifier> uint rds(Verifier* v, Graph* g, algorithm_run& runtime)
{

  if(false && g->nr_nodes >= 500)
  {
    runtime.valid    = true;
    runtime.last_i   = 1;
    runtime.value    = 19930203;
    runtime.complete = true;
    return 19930203;
  }
  #pragma omp parallel
  {
    #pragma omp single
    fprintf(stderr, "OpenMP: using up to %d threads.\n", omp_get_num_threads());
  }

  uint time_lim = runtime.time_limit;
  auto start = std::chrono::steady_clock::now(); // C++11 only
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
  timeSetEvent(time_lim*1000, 1000, handle_sigarlm, NULL, TIME_ONESHOT);
#else
  signal(SIGALRM, handle_sigalrm);
  if(time_lim > 0)
    alarm(time_lim);
#endif
  should_exit = false;
  uint n = g->nr_nodes;
  // order V
  lb = 0; // best solution size found so far
  uint* mu = new uint[n];
  for(uint i = 0; i < n; ++i)
    mu[i] = 0;

  int i;
  std::vector<vertex_set> c(g->nr_nodes);
  for(vertex_set& vs: c)
      vs.reserve(g->nr_nodes);

  // init verifiers for each thread
  std::vector<Verifier*> verifiers;
  #pragma omp parallel
  {
    #pragma omp single
    {
    uint num_threads = omp_get_num_threads();
    verifiers.resize(num_threads);
    for(uint i = 0; i < num_threads; ++i)
      verifiers[i] = v->clone();
    }
  }

  vertex_set p; p.reserve(g->nr_nodes);

  for(i = n-1; i >= 0 && !should_exit; --i)
  {
    // form candidate set
    // take vertices from v \in {i+1, n} for which pair (i,v) satisfies \Pi
    // first iteration c is empty, that must set bound to 1
    for(vertex_set& vs: c)
      vs.clear();
    auto& curC = c[0];

    for(uint j = i+1; j < n; ++j)
      if(v->check_pair(i, j))
        curC.add_vertex(j, g->weight(j));

    p.clear();
    p.add_vertex(i, g->weight(i));
    // run for level = 0 manually with respect to the thread number
    if(curC.empty())
    {
      if(p.weight > lb)
      {
        mu[i] = p.weight;
        runtime.certificate = p.vertices;
      }
      else
        mu[i] = lb;
    } else {
    should_return = false;
    #pragma omp parallel
    {
      uint thread_i = omp_get_thread_num();
      uint num_threads = omp_get_num_threads();

      if(num_threads > verifiers.size())
      {
        printf("Thread number changed? Need %lu got %u\n", verifiers.size(), num_threads);
        should_exit = true;
      } else {

      Verifier* v_ = verifiers[thread_i];
      v_->init_aux(i, curC);
      std::vector<vertex_set> c_(c);
      vertex_set p_(p);

      uint mu_i = 0;

      auto& curC_ = c_[0];
      for(uint c_i = thread_i; c_i < curC_.size() && !should_exit; c_i += num_threads) // split by threads
      {
        // adjust weight_c
        // we remove nodes [0; c_i), adjust weight accordingly
        for(uint j = 0; j < num_threads; ++j)
          if(c_i >= j+1)
            curC_.weight -= g->weight(c_i - j - 1);

        if(curC_.weight + p_.weight <= lb) // Prune 1
        {
          mu_i = lb;
          break;
        }

        uint i_ = curC_[c_i];
        if(mu[i_] + p_.weight <= lb) // Prune 2
        {
          mu_i = lb;
          break;
        } else {
          v_->prepare_aux(p_, i_, curC_, c_i+1);
          p_.add_vertex(i_, g->weight(i_));
          auto& nextC_ = c_[1];
          nextC_.clear();
          for(uint it2 = c_i+1; it2 < curC_.size(); ++it2)
          {
            uint u = curC_[it2];
            if(v_->check(p_, i_, u)) //TODO only swap check?
              nextC_.add_vertex(u, g->weight(u));
          }
          find_max(c_, p_, mu, v_, g, runtime.certificate, 1);
          p_.pop_vertex(g->weight(i_));
          v_->undo_aux(p_, i_, curC_, c_i+1);
        }
      }
      mu_i = lb;
      if(mu_i > mu[i])
      {
        #pragma omp critical (mu_update)
        {
          mu[i] = std::max(mu_i, mu[i]);
        }
      }

      v_->free_aux();
      }
    } // pragma omp parallel
    }
    if(runtime.allout || i == (int)g->nr_nodes-1 || mu[i] != mu[i+1] || i < (int)g->nr_nodes/2)
      fprintf(stderr, "i = %u (%d/%d), mu = %d\n", i, (g->nr_nodes-i), g->nr_nodes, mu[i]);
  }

  fprintf(stderr, "nr_calls = %d\n", nr_calls);

  runtime.valid    = true;
  runtime.last_i   = i+1;
  runtime.value    = mu[i+1];
  runtime.complete = ((i+1)==0);
  runtime.time     = std::chrono::steady_clock::now() - start;

  delete [] mu;
  return runtime.value;
}

#endif

