#ifndef __ORDERS_H__
#define __ORDERS_H__

#include <vector>
class graph;

enum class ordering {
  None, Degree, Degeneracy, Neighborhood, NColor, Cliquer
};

std::vector<uint> reorder_none(const graph* const g);
std::vector<uint> reorder_degree(const graph* const g);
std::vector<uint> reorder_degeneracy(const graph* const g);
std::vector<uint> reorder_rev(const graph* const g);
std::vector<uint> reorder_2nb(const graph* const g);
std::vector<uint> reorder_color(uint s, const graph* const g);
std::vector<uint> reorder_weight(const graph* const g);
std::vector<uint> reorder_cliquer(const graph* const g);

#endif
