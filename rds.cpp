#include "rds.h"
#include <vector>
#include <algorithm>
#include <ctime>
#include <cstdio>
#include <atomic>
#include <chrono>
#include <cstdlib>
#include <omp.h>

#include <csignal> // Display best result for SIGINT before exit

#ifndef max
#define max(a,b) (((a)>(b))?(a):(b))
#endif

using namespace std;

typedef unsigned int uint;

atomic_uint lb;

// for debug
void print_cont(const vector<uint>& c)
{
  for(auto it = c.begin(); it != c.end(); ++it)
    printf("%u ", *it);
  printf("\n");
}

void print_lb_atomic(int signal)
{
  printf("\nReceived SIGINT\n");
  printf("Best lower bound found: %u\n", lb.load());
  exit(0);
}

atomic_uint iter (0);
atomic_bool should_exit (false);

uint find_max(vector<vector <uint> >& c, vector<uint>& weight_c, vector<uint>& p, uint& weight_p, const atomic_uint* mu, verifier *v, graph* g, vector<uint>& res, int level, const chrono::time_point<chrono::steady_clock> start, const uint time_lim)
{
  if(should_exit)
    return lb;
  if(c[level].size() == 0)
  {
    if(weight_p > lb)
    {
      //TODO atomic copy
//      res = p; //copy
      return weight_p;
    }
    else
      return lb;
  }

  for(uint c_i = 0; c_i < c[level].size(); ++c_i)
  {
    iter++;
    if(iter % 1000 == 0 && time_lim > 0)
    {
      chrono::duration<double> d = chrono::steady_clock::now() - start;
      if(d.count() >= (double)time_lim)
      {
        should_exit = true;
        return lb;
      }
    }

    if(weight_c[level] + weight_p <= lb) // Prune 1
    {
      return lb;
    }
    uint i = c[level][c_i];
    if(mu[i].load() + weight_p <= lb) // Prune 2
    {
      return lb;
    }
    weight_c[level] -= g->weight(i);
//    NB: exploit that we adding only 1 vertex to p
//    thus verifier can prepare some info using prev calculations
    v->prepare_aux(g, p, i, c[level]);
    p.push_back(i); weight_p += g->weight(i);
    c[level+1].resize(0); weight_c[level+1] = 0;
    for(uint it2 = c_i; it2 < c[level].size(); ++it2)
    {
      if(c[level][it2] != i && v->check(g, p, c[level][it2]))
      {
        c[level+1].push_back(c[level][it2]);
        weight_c[level+1] += g->weight(c[level][it2]);
      }
    }
    lb = find_max(c, weight_c, p, weight_p, mu, v, g, res, level+1, start, time_lim);
    p.pop_back(); weight_p -= g->weight(i);
    v->undo_aux(g, p, i, c[level]);
  }
  return lb;
}

uint rds(verifier* v, graph* g, vector<uint>& res, uint time_lim)
{
  chrono::time_point<chrono::steady_clock> start = chrono::steady_clock::now(); // C++11 only
  should_exit = false;
  uint n = g->nr_nodes;
  // order V
  lb = 0; // best solution size found so far
  atomic_uint* mu = new atomic_uint[n];
  for(uint i = 0; i < n; ++i)
    mu[i] = 0;

  int i;
  for(i = n-1; i >= 0 && !should_exit; --i)
  {
    // form candidate set
    // take vertices from v \in {i+1, n} for which pair (i,v) satisfies \Pi
    // first iteration c is empty, that must set bound to 1
    vector<vector<uint> > c(g->nr_nodes); vector<uint> weight_c(g->nr_nodes, 0);
    for(uint j = 0; j < g->nr_nodes; ++j)
    {
      c[j].reserve(g->nr_nodes);
      c[j].resize(0);
    }
    for(uint j = i+1; j < n; ++j)
    {
      if(v->check_pair(g, i, j))
      {
        // add to C
        c[0].push_back(j);
        weight_c[0] += g->weight(j);
      }
    }
    vector<uint> p; uint weight_p = 0;
    p.push_back(i); weight_p += g->weight(i);
    fprintf(stderr, "i = %u, c.size = %lu, ", i, c[0].size());
    // run for level = 0 manually with respect to the thread number
    if(c[0].size() == 0)
    {
      if(weight_p > lb)
      {
        mu[i] = weight_p;
        res = p;
      }
      else
        mu[i] = lb.load();
    } else {
    #pragma omp parallel
    {
      // clone for separate threads
      verifier* v_ = v->clone();
      v_->init_aux(g, i, c[0]);
      vector<vector<uint> > c_(c); vector<uint> weight_c_(weight_c);
      vector<uint> p_(p); uint weight_p_ = weight_p;

      uint thread_i = omp_get_thread_num();
      uint num_threads = omp_get_num_threads();

      uint mu_i = 0;
 
      for(uint c_i = thread_i; c_i < c_[0].size() && !should_exit; c_i += num_threads) // split by threads
      {
        // adjust weight_c
        // we remove nodes [0; c_i), adjust weight accordingly
        for(uint j = 0; j < num_threads; ++j)
          if(c_i >= j+1)
            weight_c_[0] -= g->weight(c_i - j - 1);
        if(weight_c_[0] + weight_p_ <= lb) // Prune 1
        {
          mu_i = lb;
          break;
        }
        uint i_ = c_[0][c_i];
        if(mu[i_].load() + weight_p_ <= lb) // Prune 2
        {
          mu_i = lb;
          break;
        } else {
          v_->prepare_aux(g, p_, i_, c_[0]);
          p_.push_back(i_); weight_p_ += g->weight(i_);
          c_[1].resize(0); weight_c_[1] = 0;
          for(uint it2 = c_i; it2 < c_[0].size(); ++it2)
          {
            if(c_[0][it2] != i_ && v->check(g, p_, c_[0][it2])) //TODO only swap check?
            {
              c_[1].push_back(c_[0][it2]);
              weight_c_[1] += g->weight(c_[0][it2]);
            }
          }
          lb = max(find_max(c_, weight_c_, p_, weight_p_, mu, v_, g, res, 1, start, time_lim), lb.load());
          p_.pop_back(); weight_p_ -= g->weight(i_);
          v_->undo_aux(g, p_, i_, c_[0]);
        }
      }
      mu_i = lb;
      mu[i] = max(mu_i, mu[i].load());
      v_->free_aux();
      if(time_lim > 0)
      {
        chrono::duration<double> d = chrono::steady_clock::now() - start;
        if(static_cast<uint>(d.count()) >= time_lim)
          should_exit = true;
      }
      delete v_;
//      printf("Thread %d : mu_i is %d\n", thread_i, mu_i);
    } // pragma omp parallel
    }
    fprintf(stderr, "mu[%d] = %d\n", i, mu[i].load());
  }
  printf("RDS done\n");
  uint fres = mu[i+1].load(); // last
  delete [] mu;
  chrono::duration<double> d = chrono::steady_clock::now() - start;
  printf("rds: time elapsed = %.8lf secs\n", d.count());
  return fres;
}

