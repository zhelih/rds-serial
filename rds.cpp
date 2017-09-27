#include "rds.h"
#include <vector>
#include <algorithm>
#include <ctime>
#include <cstdio>
#include <atomic>
#include <chrono>
#include <cstdlib>

#include <csignal> // Display best result for SIGINT before exit

#ifndef max
#define max(a,b) (((a)>(b))?(a):(b))
#endif

using namespace std;

typedef unsigned int uint;

static uint lb;

atomic_uint lb_a;

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
  printf("Best lower bound found: %u\n", lb_a.load());
  exit(0);
}

static uint iter = 0;
static bool should_exit = false;

static uint nr_prune1 = 0;
static uint nr_prune2 = 0;

uint find_max(vector<vector <uint> >& c, vector<uint>& weight_c, vector<uint>& p, uint& weight_p, const uint* mu, verifier *v, graph* g, vector<uint>& res, int level, const chrono::time_point<chrono::steady_clock> start, const uint time_lim)
{
  if(should_exit)
    return lb;
  if(c[level].size() == 0)
  {
    if(weight_p > lb)
    {
      res = p; //copy
      return weight_p;
    }
    else
      return lb;
  }

  while(c[level].size() > 0)
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
      nr_prune1++;
      return lb;
    }
    uint i = c[level][c[level].size()-1];
    if(mu[i] + weight_p <= lb) // Prune 2
    {
      nr_prune2++;
      return lb;
    }
    c[level].pop_back(); weight_c[level] -= g->weight(i);
//    NB: exploit that we adding only 1 vertex to p
//    thus verifier can prepare some info using prev calculations
    v->prepare_aux(g, p, i, c[level]);
    p.push_back(i); weight_p += g->weight(i);
    c[level+1].resize(0); weight_c[level+1] = 0;
    for(uint it2 = 0; it2 < c[level].size(); ++it2)
    {
      if(c[level][it2] != i && v->check(g, p, c[level][it2]))
      {
        c[level+1].push_back(c[level][it2]);
        weight_c[level+1] += g->weight(c[level][it2]);
      }
    }
    lb = find_max(c, weight_c, p, weight_p, mu, v, g, res, level+1, start, time_lim);
    lb_a = lb;
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
  lb_a = 0;
  uint *mu = new uint[n];

  int i;
  for(i = n-1; i >= 0; --i)
  {
    // form candidate set
    // take vertices from v \in {i+1, n} for which pair (i,v) satisfies \Pi
    // first iteration c is empty, that must set bound to 1
    vector<vector<uint> > c; vector<uint> weight_c(g->nr_nodes);
    c.resize(g->nr_nodes);
    for(uint j = 0; j < g->nr_nodes; ++j)
    {
      c[j].reserve(g->nr_nodes);
      c[j].resize(0);
      weight_c[j] = 0;
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
    reverse(c[0].begin(), c[0].end()); // for efficient deletion of min element
    vector<uint> p; uint weight_p = 0;
    p.push_back(i); weight_p += g->weight(i);
    v->init_aux(g, i, c[0]);
    fprintf(stderr, "i = %u, c.size = %lu, ", i, c[0].size());
    mu[i] = find_max(c, weight_c, p, weight_p, mu, v, g, res, 0, start, time_lim);
    fprintf(stderr, "mu[%d] = %d\n", i, mu[i]);
    v->free_aux();
    if(time_lim > 0)
    {
      chrono::duration<double> d = chrono::steady_clock::now() - start;
      if((uint)(d.count()) >= time_lim)
        break;
    }
  }
  printf("RDS done\n");
  uint fres = mu[i+1]; // last
  delete [] mu;
  chrono::duration<double> d = chrono::steady_clock::now() - start;
  printf("rds: time elapsed = %.8lf secs\n", d.count());
  printf("rds: nr_prune1 = %u, nr_prune2 = %u\n", nr_prune1, nr_prune2);
  return fres;
}
