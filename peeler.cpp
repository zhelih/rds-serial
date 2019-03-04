#include <cstdio>
#include "graph/graph_utils.hpp"
#include "graph/graph_adjacency.h"

using Graph = graph_adjacency;

int main(int argc, char* argv[])
{
  if(argc < 2)
  {
    printf("Usage: %s <.dimacs> [output]\n", argv[0]);
    return 0;
  }
  Graph* g = from_dimacs<Graph>(argv[1]);
  return 0;
}
