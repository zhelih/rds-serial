#include <vector>
#include <algorithm>
#include <iterator>
#include "graph.h"
#include "orders.h"

graph::graph(uint n): nr_nodes(n)
{
  std::fill_n(std::back_inserter(this->weights), n, 1);
  current_order = reorder_none(this);
}

void graph::apply_order(ordering order_type, bool reverse) {
  std::vector<uint> order = reorder_none(this);
  switch (order_type) {
    case ordering::None:
      break;
    case ordering::Degree:
      order = reorder_degree(this);
      break;
    case ordering::Degeneracy:
      order = reorder_degeneracy(this);
      break;
    case ordering::Neighborhood:
      order = reorder_2nb(this);
      break;
    case ordering::NColor:
      order = reorder_color(2, this);
      break;
    case ordering::Cliquer:
      order = reorder_cliquer(this);
      break;
  }
  this->reorder_custom(order);

  if (reverse) {
    order = reorder_rev(this);
    this->reorder(order);
  }
}

void graph::reorder(const std::vector<uint>& order) {
  this->reorder_weights(order);
  this->reorder_custom(order);

  std::vector<uint> new_order(nr_nodes);
  for (uint i = 0; i < nr_nodes; ++i)
    new_order[i] = this->current_order[order[i]];
  this->current_order = new_order;
}

void graph::reorder_weights(const std::vector<uint>& order) {
  std::vector<uint> order_reverse = this->reverse_order(order);
  std::vector<uint> weights_new(nr_nodes);
  for (uint v = 0; v < nr_nodes; ++v)
    weights_new[order_reverse[v]] = this->weights[v];
  this->weights = weights_new;
}

std::vector<uint> graph::reverse_order(const std::vector<uint>& order) const
{
  std::vector<uint> order_reverse(order.size());
  for (uint p = 0; p < nr_nodes; ++p)
  {
    order_reverse[order[p]] = p;
  }
  return order_reverse;
}

void graph::restore_order(std::vector<uint>& v)
{
  std::transform(v.begin(), v.end(), v.begin(), [&](const int& i){return current_order[i];});
}

void graph::read_weights(std::istream& weights)
{
  std::istream_iterator<uint> input(weights);
  std::copy_n(input, this->nr_nodes, std::back_inserter(this->weights));
}
