#ifndef __GRAPH_UTILS_HPP__
#define __GRAPH_UTILS_HPP__
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include "graph.h"

//#define DIMACS_FROM_1

template <typename T> T* from_dimacs(const char* graph_file) {
  std::ifstream source(graph_file);
  std::string line, _;
  int V, E, U, N;
  T* result = nullptr;
  int line_counter = 0;
  while (std::getline(source, line)) {
    if(line == "") continue;
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
#ifdef DIMACS_FROM_1
        if(!result->is_edge(U - 1, V - 1))
          result->add_edge(U - 1, V - 1);
#else
        if(!result->is_edge(U, V))
          result->add_edge(U, V);
#endif
      }
    }
    else {
      std::cerr<<"Failed to read graph data stream: malformed line " << line_counter << ":"<<line<<std::endl;
      return nullptr;
    }
  }
  return result;
}

template <typename T> void save_dimacs(const T* g, const char* filename) {
  FILE* f = fopen(filename, "w");
  if(!f) {
    fprintf(stderr, "Failed to open %s\n", filename);
    return;
  }
  fprintf(f, "p edge %d %d\n", g->nr_nodes, 0); // FIXME nr_edges?
  //FIXME efficiency
  for(int i = 0; i < g->nr_nodes; ++i)
    for(int j = i+1; j < g->nr_nodes; ++j)
      if(g->is_edge(i, j))
        fprintf(f, "e %d %d\n", i+1, j+1);
  fclose(f);
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
