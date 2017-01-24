#include <cstdio>
#include <string>
#include <algorithm>
#include "rds.h"
#include "graph.h"
#include "verifier.h"

using namespace std;

int main(int argc, char* argv[])
{
  printf("Russian-Doll-Search\n");

  if(argc < 2) {

  graph* g = new graph(5);
  g->add_edge(0,1);
  g->add_edge(1,2);
  g->add_edge(1,3);
  g->add_edge(2,3);
  g->add_edge(2,4);

  g->reorder_degree();
  g->print_mapka();
  vector<uint> r;
  uint res = rds(new clique(), g, r);
  printf("-----------------------\nRDS returned %u\n", res);
  res = rds(new stable(), g, r);
  printf("-----------------------\nRDS returned %u\n", res);
  res = rds(new iuc(), g, r);
  printf("-----------------------\nRDS returned %u\n", res);
  res = rds(new defective_clique(1), g, r);
  printf("-----------------------\nRDS returned %u\n", res);
  delete g;
  printf("\n--------------\n\n");
  g = new graph(6);
  g->add_edge(0,1);
  g->add_edge(0,2);
  g->add_edge(0,3);
  g->add_edge(0,4);
  g->add_edge(0,5);
  res = rds(new clique(), g, r);
  printf("-----------------------\nRDS returned %u\n", res);
  res = rds(new stable(), g, r);
  printf("-----------------------\nRDS returned %u\n", res);
  res = rds(new iuc(), g, r);
  printf("-----------------------\nRDS returned %u\n", res);
  res = rds(new defective_clique(1), g, r);
  printf("-----------------------\nRDS returned %u\n", res);

  } else {
    graph * g = from_dimacs(argv[1]);
//    g -> reorder_degree();
    g->reorder_random();
    g -> reorder_rev(); // revert the order
    g -> reorder_rev(); // revert the order
//    g->reorder_color(3);
    vector<uint> res_p;
//    auto v = new defective_clique(1);
//    auto v = new iuc();
    auto v = new clique();
    uint res = rds(v, g, res_p);
    if(v->check_solution(g, res_p))
      printf("CORRECT!\n");
    else
      printf("NOT CORRECT GFMO!!!!\n");
    g->restore_order(res_p);
    sort(res_p.begin(), res_p.end());
    printf("---------------------------------\nSolution:\n");
    for(uint i = 0; i < res_p.size(); ++i)
      printf("%u ", res_p[i]);
    printf("\nRDS returned res = %d\n", res);
    delete g;
  }


  return 0;
}
