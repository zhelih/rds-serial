#include "graph_matrix.h"
#include <vector>

using namespace std;

void graph_matrix::separate_vertex(uint v) {
  for (uint u = 0; u < this->nr_nodes; ++u) {
    adj[v][u] = adj[u][v] = 0;
  }
}

void graph_matrix::reorder_custom(const std::vector<uint>& order) {
  std::vector<uint> order_reverse = this->reverse_order(order);
  bool* adj_new = (bool*)malloc(sizeof(bool)*nr_nodes*nr_nodes);
  for (uint v = 0; v < nr_nodes; ++v)
    for (uint u = 0; u < nr_nodes; ++u)
      adj_new[order_reverse[v]*nr_nodes + order_reverse[u]] = this->adj[v][u];

  for (uint v = 0; v < nr_nodes; ++v)
    for (uint u = 0; u < nr_nodes; ++u)
      this->adj[v][u] = adj_new[v*nr_nodes + u];

  free(adj_new);
}
