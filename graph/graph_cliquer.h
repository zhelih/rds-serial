#ifndef _GRAPH_CLIQUER_H
#define _GRAPH_CLIQUER_H

#include <vector>
#include <unordered_set>
#include "graph.h"
#include "orders.h"
#include "set.h"

#define GRAPH_IS_EDGE_FAST(g,i,j)  (SET_CONTAINS_FAST((g)->edges[(i)],(j)))
#define GRAPH_IS_EDGE(g,i,j) (((i)<((g)->n))?SET_CONTAINS((g)->edges[(i)], \
							  (j)):FALSE)
#define GRAPH_ADD_EDGE(g,i,j) do {            \
	SET_ADD_ELEMENT((g)->edges[(i)],(j)); \
	SET_ADD_ELEMENT((g)->edges[(j)],(i)); \
} while (FALSE)
#define GRAPH_DEL_EDGE(g,i,j) do {            \
	SET_DEL_ELEMENT((g)->edges[(i)],(j)); \
	SET_DEL_ELEMENT((g)->edges[(j)],(i)); \
} while (FALSE)


class graph_cliquer: public graph {
private:
  set_t *edges;
  void reorder_custom(const std::vector<uint>& order) final override {};

public:
  graph_cliquer(uint n): graph(n) {
    this->edges = static_cast<long unsigned int**>(calloc(n, sizeof(set_t)));
    for (int i = 0; i < n; ++i) {
      this->edges[i] = set_new(n);
    }
  }
  ~graph_cliquer() {}

  inline void add_edge(uint i, uint j) final override {
    GRAPH_ADD_EDGE(this, i, j);
  }
  
  inline bool is_edge(uint i, uint j) const final override {
    return GRAPH_IS_EDGE_FAST(this, i, j);
  }

  void separate_vertex(uint i) final override {};
  
  using neighbourhood_iterator = int;

  inline neighbourhood_iterator nbh_begin(const uint v) {
    return 0;
  }

  inline neighbourhood_iterator nbh_end(const uint v) {
    return 0;
  }
};

#endif
