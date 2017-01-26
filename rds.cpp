#include "rds.h"
#include <vector>
#include <algorithm>
#include <ctime>
#include <cstdio>
#include <atomic>

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

uint find_max(vector<vector <uint> >& c, vector<uint>& p, const uint* mu, verifier *v, graph* g, vector<uint>& res, void* aux, int level)
{
//  printf("debug: running find_max with c.size = %lu, p.size = %lu\n", c.size(), p.size());
  if(c[level].size() == 0)
  {
    if(p.size() > lb)
    {
      res = p; //copy
      return p.size();
    }
    else
      return lb;
  }

  while(c[level].size() > 0)
  {
    if(c[level].size() + p.size() < lb) // Prune 1
      return lb;
    uint i = c[level][c[level].size()-1];
    if(mu[i] + p.size() < lb) // Prune 2
      return lb;
    c[level].pop_back();
//    NB: exploit that we adding only 1 vertex to p
//    thus verifier can prepare some info using prev calculations
    v->prepare_aux(g, p, i, c[level], aux);
    p.push_back(i);
    c[level+1].resize(0);
    for(uint it2 = 0; it2 < c[level].size(); ++it2)
    {
      if(c[level][it2] != i && v->check(g, p, c[level][it2], aux))
      {
        c[level+1].push_back(c[level][it2]);
      }
    }
/*    if(c_new.size() == 0) {
      if(p.size() > lb)
        lb = p.size();
    } else */
    lb = find_max(c, p, mu, v, g, res, aux, level+1);
    lb_a = lb;
    p.pop_back();
//    if(aux != 0)
    v->undo_aux(g, p, i, c[level], aux);
  }
  return lb;
}

uint rds(verifier* v, graph* g, vector<uint>& res, uint time_lim)
{
  clock_t start = clock();
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
    vector<vector<uint> > c;
    c.resize(g->nr_nodes);
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
      }
    }
    reverse(c[0].begin(), c[0].end()); // for efficient deletion of min element
    vector<uint> p;
    p.push_back(i);
    void* aux = v->init_aux(g, i, c[0]);
    printf("i = %u, c.size = %lu, ", i, c[0].size());
    mu[i] = find_max(c, p, mu, v, g, res, aux, 0);
    printf("mu[%d] = %d\n", i, mu[i]);
    v->free_aux(aux);
    if(time_lim > 0 && (clock()-start)/CLOCKS_PER_SEC >= time_lim)
      break;
  }
  printf("RDS done\n");
  uint fres = mu[i+1]; // last
  delete [] mu;
  clock_t end = clock();
  printf("rds: time elapsed = %lf secs\n", (double)(end-start)/CLOCKS_PER_SEC);
  return fres;
}
