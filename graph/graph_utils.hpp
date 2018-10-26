#ifndef __GRAPH_UTILS_HPP__
#define __GRAPH_UTILS_HPP__
#include <iostream>
#include <sstream>
#include <string>
#include "graph.h"

template <typename T> T* from_dimacs(std::istream& source) {
  std::string line, _;
  int V, E, U, N;
  T* result = nullptr;
  int line_counter = 0;
  while (std::getline(source, line)) {
    std::istringstream linestream(line);
    ++line_counter;
    std::string type;
    linestream >> type;
    if (type == "c") continue;
    else if (type == "p") {
      linestream >> _ >> N >> E;
      result = new T(N);
    }
    else if (type == "e") {
      if (!result) {
        std::cerr<<"Failed to read graph data stream"<<std::endl;
        return nullptr;
      }
      else {
        linestream >> U >> V;
        result->add_edge(U - 1, V - 1);
      }
    }
    else {
      std::cerr<<"Failed to read graph data stream: malformed line " << line_counter << ":"<<line<<std::endl;
      return nullptr;
    }
  }
  return result;
}

template <typename T> T* complement(const graph* original) {
  T* g = new T(original->nr_nodes);
  for(unsigned int i = 0; i < original->nr_nodes; ++i)
    for(unsigned int j = i+1; j < original->nr_nodes; ++j)
      if (!(original->is_edge(i, j)))
        g->add_edge(i,j);
  return g;
}
#endif
