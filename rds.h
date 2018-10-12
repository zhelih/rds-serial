#ifndef _RDS_H
#define _RDS_H

#include <vector>
#include <string>
#include <chrono>
#include "verifiers/verifier.hpp"

typedef unsigned int uint;

class graph;

struct algorithm_run {
  std::string graphname;
  bool valid = false;
  bool complete = false;
  bool reverse = false;
  bool complement = false;
  bool correct = false;

  unsigned int time_limit;

  std::chrono::duration<double> time;
  unsigned int value;
  unsigned int last_i;

  std::vector<uint> certificate;
};

struct vertex_set {
  std::vector<uint> vertices;
  uint weight = 0;

  inline void add_vertex(const uint& v, const uint& w) {
    vertices.push_back(v);
    weight += w;
  }

  inline void pop_vertex(const uint& w) {
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
uint rds(verifier* v, graph* g, algorithm_run& runtime);
void print_lb_atomic(int signal);

#endif
