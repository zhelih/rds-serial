#ifndef _RDS_H
#define _RDS_H

#include <vector>
#include "graph.h"
#include "verifier.h"

typedef unsigned int uint;

struct VertexSet {
  std::vector<uint> vertices;
  uint weight = 0;

  inline void addVertex(const uint& v, const uint& w) {
    vertices.push_back(v);
    weight += w;
  }

  inline void popVertex(const uint& w) {
    vertices.pop_back();
    weight -= w;
  }

  inline size_t size() const {
    return vertices.size();
  }

  inline bool empty() const {
    return vertices.empty();
  }

  inline void clear() {
    vertices.resize(0);
    weight = 0;
  }

  inline void reserve(const size_t& size) {
    vertices.reserve(size);
  }

  operator std::vector<uint>&() { return vertices; }

  inline const uint& operator[](const size_t& idx) const { return vertices[idx]; }
  inline uint& operator[](const size_t& idx) { return vertices[idx]; }
};

//Russian-Doll-Search
// input: verifier v, graph g, vector for storing the result res
// time limit in seconds
// output: maximum subgraph size in g satisfying v
// res contains the solution, use graph::restore_order to get original nodes
uint rds(verifier* v, graph*g, std::vector<uint>& res, uint time_lim = 0);

void print_lb_atomic(int signal);

#endif
