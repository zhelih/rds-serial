#include <cstdio>
#include <vector>
#include <algorithm>
#include "graph/graph_utils.hpp"

using namespace std;

struct ez_graph
{
  vector<vector<int> > nb;
  int nr_nodes;
  ez_graph(int n) : nb(n), nr_nodes(n) {}
  void add_edge(int i, int j) { nb[i].push_back(j); nb[j].push_back(i); }
  bool is_edge(int i, int j) const { return find(nb[i].begin(), nb[i].end(), j) != nb[i].end(); }
};

int main(int argc, char* argv[])
{
  if(argc < 3)
  {
    printf("Usage: %s <n> <.dimacs> [output]\n", argv[0]);
    return 0;
  }
  unsigned int n = atoi(argv[1]);
  ez_graph* g = from_dimacs<ez_graph>(argv[2]);

  vector<int> deleted(g->nr_nodes, 0);
  int nr_deleted = 0;

  bool more = true;
  while(more)
  {
    more = false;
    for(int i = 0; i < g->nr_nodes; ++i)
    {
      if(!deleted[i] && g->nb[i].size() < n) // FIXME <=
      {
        more = true;
        deleted[i] = 1;
        nr_deleted++;
        for(int nb : g->nb[i])
          if(!deleted[nb])
            g->nb[nb].erase(find(g->nb[nb].begin(), g->nb[nb].end(), i)); // lol
      }
    }
  }

  string out_fname = "peeled.dimacs";
  if(argc > 3)
    out_fname = argv[3];

  printf("Peeled %d vertices, saving to %s\n", nr_deleted, out_fname.c_str());

  // do map
  vector<int> offset(g->nr_nodes, 0);
  int cur = 0;
  for(int i = 0; i < g->nr_nodes; ++i)
  {
    offset[i] = cur;
    if(deleted[i])
      cur++;
  }

  FILE* f = fopen(out_fname.c_str(), "w");
  if(!f)
  {
    fprintf(stderr, "Failed to open %s\n", out_fname.c_str());
    return 1;
  }

  fprintf(f, "p edge %d 0\n", g->nr_nodes - nr_deleted); //FIXME nr_edges

  for(int i = 0; i < g->nr_nodes; ++i)
  {
    if(!deleted[i])
      for(int nb : g->nb[i])
        if(i < nb && !deleted[nb])
          fprintf(f, "e %d %d\n", i - offset[i] + 1, nb - offset[nb] + 1);
  }
  fclose(f);
  return 0;
}
