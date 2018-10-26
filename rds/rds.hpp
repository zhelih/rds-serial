#ifndef _RDS_H
#define _RDS_H

#include <vector>
#include <string>
#include "../verifiers/verifier.hpp"
#include "rds_utils.hpp"
#include <vector>
#include <algorithm>
#include <ctime>
#include <cstdio>
#include <atomic>
#include <chrono>
#include <cstdlib>
#include <omp.h>
#include <atomic>
#include <csignal>

std::atomic_bool should_exit (false);
std::atomic_uint lb;

void print_lb_atomic(int signal)
{
  fprintf(stderr, "\nReceived SIGINT\n");
  fprintf(stderr, "Best lower bound found: %u\n", lb.load());
  exit(0);
}

template <typename Verifier> void find_max(std::vector<vertex_set>& c, vertex_set& p, const uint* mu, Verifier *v, graph* g, std::vector<uint>& res, int level, const std::chrono::time_point<std::chrono::steady_clock> start, const uint time_lim) {
  auto& curC = c[level];
  if(should_exit)
    return;
  if(curC.empty())
  {
    if(p.weight > lb)
    {
      #pragma omp critical (lbupdate)
      {
      res = p.vertices; //copy
      lb.store(std::max(lb.load(), p.weight));
      }
      return;
    }
    else
      return;
  }

  auto& nextC = c[level+1];
  for(uint c_i = 0; c_i < curC.size(); ++c_i)
  {
    if(curC.weight + p.weight <= lb) // Prune 1
    {
      return;
    }

    uint i = curC[c_i];
    if(mu[i] + p.weight <= lb) // Prune 2
    {
      return;
    }

    curC.weight -= g->weight(i);
//    NB: exploit that we adding only 1 vertex to p
//    thus verifier can prepare some info using prev calculations
    v->prepare_aux(p, i, curC);
    p.add_vertex(i, g->weight(i));
    nextC.clear();
    for(uint it2 = c_i; it2 < curC.size(); ++it2)
    {
      auto&& u = curC[it2];
      if(u != i && v->check(p, u))
      {
        nextC.add_vertex(u, g->weight(u));
      }
    }
    find_max(c, p, mu, v, g, res, level+1, start, time_lim);
    p.pop_vertex(g->weight(i));
    v->undo_aux(p, i, curC);
  }
  return;
}

template <typename Verifier> uint rds(Verifier* v, graph* g, algorithm_run& runtime)
{
  uint time_lim = runtime.time_limit;
  auto start = std::chrono::steady_clock::now(); // C++11 only
  should_exit = false;
  uint n = g->nr_nodes;
  // order V
  lb = 0; // best solution size found so far
  uint* mu = new uint[n];
  for(uint i = 0; i < n; ++i)
    mu[i] = 0;

  int i;
  for(i = n-1; i >= 0 && !should_exit; --i)
  {
    // form candidate set
    // take vertices from v \in {i+1, n} for which pair (i,v) satisfies \Pi
    // first iteration c is empty, that must set bound to 1
    std::vector<vertex_set> c(g->nr_nodes);
    for(auto&& vs: c)
      vs.reserve(g->nr_nodes);
    auto& curC = c[0];

    for(uint j = i+1; j < n; ++j)
      if(v->check_pair(i, j))
        curC.add_vertex(j, g->weight(j));

    vertex_set p;
    p.add_vertex(i, g->weight(i));
    fprintf(stderr, "i = %u, c.size = %lu, ", i, curC.size());
    // run for level = 0 manually with respect to the thread number
    if(curC.empty())
    {
      if(p.weight > lb)
      {
        mu[i] = p.weight;
        runtime.certificate = p.vertices;
      }
      else
        mu[i] = lb.load();
    } else {
    #pragma omp parallel
    {
      // clone for separate threads
      auto v_ = v->clone();
      v_->init_aux(i, curC);
      std::vector<vertex_set> c_(c);
      vertex_set p_(p);

      uint thread_i = omp_get_thread_num();
      uint num_threads = omp_get_num_threads();

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
          mu_i = lb.load();
          break;
        }

        uint i_ = curC_[c_i];
        if(mu[i_] + p_.weight <= lb) // Prune 2
        {
          mu_i = lb.load();
          break;
        } else {
          v_->prepare_aux(p_, i_, curC_);
          p_.add_vertex(i_, g->weight(i_));
          auto& nextC_ = c_[1];
          nextC_.clear();
          for(uint it2 = c_i; it2 < curC_.size(); ++it2)
          {
            uint u = curC_[it2];
            if(u != i_ && v_->check(p_, u)) //TODO only swap check?
              nextC_.add_vertex(u, g->weight(u));
          }
          find_max(c_, p_, mu, v_, g, runtime.certificate, 1, start, time_lim);
          p_.pop_vertex(g->weight(i_));
          v_->undo_aux(p_, i_, curC_);
        }
      }
      mu_i = lb.load();
      #pragma omp critical
      {
        mu[i] = std::max(mu_i, mu[i]);
      }
      if(time_lim > 0)
      {
        auto d = std::chrono::steady_clock::now() - start;
        if(static_cast<uint>(d.count()) >= time_lim)
          should_exit = true;
      }
      delete v_;
    } // pragma omp parallel
    }
    fprintf(stderr, "mu[%d] = %d\n", i, mu[i]);
  }

  runtime.valid    = true;
  runtime.last_i   = i+1;
  runtime.value    = mu[i+1];
  runtime.complete = ((i+1)==0);
  runtime.time     = std::chrono::steady_clock::now() - start;

  delete [] mu;
  return runtime.value;
}

#endif
