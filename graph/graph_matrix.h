#ifndef _GRAPH_MATRIX_H
#define _GRAPH_MATRIX_H

#include <vector>
#include "graph.h"
#include "orders.h"
#include <iterator>

class graph_matrix: public graph {
private:
  std::vector<std::vector<uint>>  adj;
  void reorder_custom(const std::vector<uint>& order) final override;

public:
  graph_matrix(uint n): graph(n), adj(n, std::vector<uint>(n, false)) {}
  ~graph_matrix() {}

  inline void add_edge(uint i, uint j) final override { adj[i][j] = adj[j][i] = 1; };
  inline bool is_edge(uint i, uint j) const final override {return adj[i][j];}
  void separate_vertex(uint i) final override;

  class neighbourhood_iterator {
    private:
      uint m_of_vertex;
      uint m_to_vertex;
      const graph_matrix* graph;
    public:
      using self_type = neighbourhood_iterator;
      using value_type = uint;
      using reference = uint&;
      using pointer = uint;
      using iterator_category = std::input_iterator_tag;
    
      neighbourhood_iterator(const graph_matrix* g, const uint v, const uint u): m_of_vertex(v), m_to_vertex(u), graph(g) {}

      void operator++() {
        this-> m_to_vertex = graph->find_next_neighbour(m_of_vertex, m_to_vertex);
      }

      reference operator*() {
        return m_to_vertex;
      }

      bool operator==(const self_type &rhs) {
        return (this->graph == rhs.graph && this->m_of_vertex == rhs.m_of_vertex && this->m_to_vertex == rhs.m_to_vertex);
      }

      bool operator!=(const self_type &rhs) {
        return !(*this == rhs);
      }
  };

  inline uint find_next_neighbour(uint of, uint current) const {
    for (uint u = current + 1; u < this->nr_nodes; ++u) {
      if (adj[of][u])
        return u;
    }
    return this->nr_nodes;
  }

  inline neighbourhood_iterator nbh_begin(const uint v) {
    for (uint u = 0; u < this->nr_nodes; ++u) {
      if (adj[v][u])
        return neighbourhood_iterator(this, v, u);
    }
    return this->nbh_end(v);
  }

  inline neighbourhood_iterator nbh_end(const uint v) {
    return neighbourhood_iterator(this, v, this->nr_nodes);
  }
};

#endif
