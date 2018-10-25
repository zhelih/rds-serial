#include "graph_adjacency.h"
#include <cstdio>
#include <string>
#include <vector>
#include <algorithm>
#include <set>
#include <iterator>
#include <cstdlib>

void graph_adjacency::separate_vertex(uint v) {
  adj[v].clear();
  for (auto& list: adj) list.erase(v);
}

void graph_adjacency::reorder_custom(const std::vector<uint>& order)
{
  std::vector<uint> order_reverse = this->reverse_order(order);
  for (uint v = 0; v < this->nr_nodes; ++v) {
    std::unordered_set<uint> new_adj;
    std::transform(this->adj[v].begin(), this->adj[v].end(), std::inserter(new_adj, new_adj.end()), [&](const uint &v){return order_reverse[v];});
    this->adj[v] = new_adj;
  }

  std::vector<std::unordered_set<uint>> new_adj(this->nr_nodes);
  for (uint v = 0; v < this->nr_nodes; ++v) {
    new_adj[order_reverse[v]] = adj[v];
  }
  adj = new_adj;
}
