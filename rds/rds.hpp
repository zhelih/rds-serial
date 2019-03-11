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

template <typename Verifier> void find_max(vertex_set** c, vertex_set* p, const uint* mu, Verifier *v, Graph* g, std::vector<uint>& res, int level) {
  if(should_return || should_exit)
    return;
  vertex_set* curC = c[level];

  if(curC->empty())
  {
    if(p->weight <= lb)
      return;
    #pragma omp critical (lbupdate)
    {
      p->vertices.to_std(res);; //copy
      should_return = true;
      lb = std::max(lb, p->weight);
    }
    return;
  }

  vertex_set* nextC = c[level+1];
  for (int v_it = 0; v_it < curC->size(); ++v_it) {
    if(curC->weight + p->weight <= lb) // Prune 1
      return;
    if(mu[curC->vertices[v_it]] + p->weight <= lb) // Prune 2
      return;

    uint v_weight = g->weight(curC->vertices[v_it]);
    curC->weight -= v_weight;

//    NB: exploit that we adding only 1 vertex to p
//    thus verifier can prepare some info using prev calculations
    v->prepare_aux(p->vertices, curC->vertices[v_it], curC->vertices, v_it+1);
    nextC->clear();
    p->add_vertex(curC->vertices[v_it], v_weight);
    for(int u_it = v_it+1; u_it < curC->size(); ++u_it)
      if(v->check(p->vertices, curC->vertices[v_it], curC->vertices[u_it]))
        nextC->add_vertex(curC->vertices[u_it], g->weight(curC->vertices[u_it]));

    if (nextC->weight + p->weight > lb) // continue if <=
      find_max(c, p, mu, v, g, res, level+1);

    p->pop_vertex(v_weight);
    v->undo_aux(p->vertices, curC->vertices[v_it], curC->vertices, v_it+1);

    if(should_return)
      return;
  }
  return;
}

template <typename Verifier> uint rds(Verifier* v, Graph* g, algorithm_run& runtime)
{

  #pragma omp parallel
  {
    #pragma omp single
    fprintf(stderr, "OpenMP: using up to %d threads.\n", omp_get_num_threads());
  }

  uint time_lim = runtime.time_limit;
  auto start = std::chrono::steady_clock::now(); // C++11 only
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
  if(time_lim > 0)
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
  vertex_set** c = new vertex_set*[g->nr_nodes];
  for(i = 0; i < g->nr_nodes; ++i)
      c[i] = new vertex_set(g->nr_nodes);

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

  vertex_set* p = new vertex_set(g->nr_nodes);

  for(i = n-1; i >= 0 && !should_exit; --i)
  {
    // form candidate set
    // take vertices from v \in {i+1, n} for which pair (i,v) satisfies \Pi
    // first iteration c is empty, that must set bound to 1
    for(int j = 0; j < g->nr_nodes; ++j)
      c[j]->clear();
    vertex_set* curC = c[0];

    for(uint j = i+1; j < n; ++j)
      if(v->check_pair(i, j))
        curC->add_vertex(j, g->weight(j));

    p->clear();
    p->add_vertex(i, g->weight(i));
    // run for level = 0 manually with respect to the thread number
    if(curC->empty())
    {
      if(p->weight > lb)
      {
        mu[i] = p->weight;
        p->vertices.to_std(runtime.certificate);
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
      v_->init_aux(i, curC->vertices);
      vertex_set** c_ = new vertex_set*[g->nr_nodes];
      for(int j = 0; j < g->nr_nodes; ++j)
        c_[j] = c[j]->clone();
      vertex_set* p_ = p->clone();

      uint mu_i = 0;

      vertex_set* curC_ = c_[0];
      for(int c_i = thread_i; c_i < curC_->size() && !should_exit; c_i += num_threads) // split by threads
      {
        // adjust weight_c
        // we remove nodes [0; c_i), adjust weight accordingly
        for(uint j = 0; j < num_threads; ++j)
          if((uint)c_i >= j+1)
            curC_->weight -= g->weight(c_i - j - 1);

        if(curC_->weight + p_->weight <= lb) // Prune 1
        {
          mu_i = lb;
          break;
        }

        uint i_ = curC_->vertices[c_i];
        if(mu[i_] + p_->weight <= lb) // Prune 2
        {
          mu_i = lb;
          break;
        } else {
          v_->prepare_aux(p_->vertices, i_, curC_->vertices, c_i+1);
          p_->add_vertex(i_, g->weight(i_));
          vertex_set* nextC_ = c_[1];
          nextC_->clear();
          for(int it2 = c_i; it2 < curC_->size(); ++it2)
          {
            uint u = curC_->vertices[it2];
            if(u != i_ && v_->check(p_->vertices, i_, u)) //TODO only swap check?
              nextC_->add_vertex(u, g->weight(u));
          }
          find_max(c_, p_, mu, v_, g, runtime.certificate, 1);
          p_->pop_vertex(g->weight(i_));
          v_->undo_aux(p_->vertices, i_, curC_->vertices, c_i+1);
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
      } // pragma omp parallel
      } // else
    } // for
    if(runtime.allout || i == (int)g->nr_nodes-1 || mu[i] != mu[i+1] || i < (int)g->nr_nodes/2)
      fprintf(stderr, "i = %u (%d/%d), mu = %d\n", i, (g->nr_nodes-i), g->nr_nodes, mu[i]);
  }

  /*for(i = 0; i < g->nr_nodes; ++i)
    delete c[i];
*/
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

