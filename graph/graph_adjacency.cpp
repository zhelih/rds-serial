#include "graph_adjacency.h"
#include <cstdio>
#include <string>
#include <vector>
#include <algorithm>
#include <set>
#include <cstdlib>

void graph_adjacency::separate_vertex(uint v) {
  adj[v].clear();
  for (auto& list: adj) list.erase(v);
}

void graph_adjacency::reorder_custom(const std::vector<uint>& order)
{
/*
  std::vector<uint> order_reverse = this->reverse_order(order);
  bool* adj_new = (bool*)malloc(sizeof(bool)*nr_nodes*nr_nodes);
  for (uint v = 0; v < nr_nodes; ++v)
    for (uint u = 0; u < nr_nodes; ++u)
      adj_new[order_reverse[v]*nr_nodes + order_reverse[u]] = this->adj[v][u];

  for (uint v = 0; v < nr_nodes; ++v)
    for (uint u = 0; u < nr_nodes; ++u)
      this->adj[v][u] = adj_new[v*nr_nodes + u];

  free(adj_new);

  std::vector<uint> weights_new(nr_nodes);
  for (uint v = 0; v < nr_nodes; ++v)
    weights_new[order_reverse[v]] = this->weights[v];
  this->weights = weights_new;

  std::vector<uint> new_order(nr_nodes);
  for (uint i = 0; i < nr_nodes; ++i)
    new_order[i] = this->current_order[order[i]];
  this->current_order = new_order;
*/
}
