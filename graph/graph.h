#ifndef _GRAPH_H
#define _GRAPH_H

typedef unsigned int uint;
#include <vector>
#include <iostream>
#include "orders.h"

class graph
{
private:
  std::vector<uint> current_order;
  std::vector<uint> weights;
  virtual void reorder_custom(const std::vector<uint>& order) = 0;
  void reorder_weights(const std::vector<uint> & order);

public:
  uint nr_nodes;
  graph(uint n);
  virtual ~graph() {};

  virtual void add_edge(uint i, uint j) = 0;
  virtual bool is_edge(uint i, uint j) const = 0;
  virtual void separate_vertex(uint i) = 0;

  inline uint weight(uint i) const { return weights[i]; }
  void restore_order(std::vector<uint>& v);
  void apply_order(ordering order, bool reverse);
  std::vector<uint> reverse_order(const std::vector<uint>& order) const;
  void reorder(const std::vector<uint>& order);
  void read_weights(const char* filename);
};
#endif
