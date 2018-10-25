#include <set>
#include <vector>
#include <algorithm>
#include "graph.h"

inline bool compare_pairs(const std::pair<uint, uint> &left, const std::pair<uint, uint> &right) { return left.first > right.first; }

std::vector<uint> reorder_none(const graph* const g) // revert the order of vertices (usually used to change from small to large)
{
  std::vector<uint> order;
  std::generate_n(std::back_inserter(order), g->nr_nodes, [&](){return order.size();});
  return order;
}

std::vector<uint> reorder_degree(const graph* const g) // degree order from large to small
{
  std::vector<std::pair<uint,uint>> degrees; // pair degree, node number
  std::generate_n(std::back_inserter(degrees), g->nr_nodes, [&](){return std::make_pair(0, degrees.size());});

  for(uint i = 0; i < g->nr_nodes; ++i)
    for(uint j = 0; j < g->nr_nodes; ++j)
      if(i != j && g->is_edge(i,j))
        degrees[i].first++;

  // sort from large to small based on degree
  sort(degrees.begin(), degrees.end(), compare_pairs);

  std::vector<uint> order;
  std::generate_n(std::back_inserter(order), degrees.size(), [&](){return degrees[order.size()].second;});
  return order;
}

std::vector<uint> reorder_degeneracy(const graph* const g) {
  std::vector<bool> used(g->nr_nodes, false);
  std::vector<uint> order(g->nr_nodes);
  uint counter = 0, min_counter = g->nr_nodes+1, selected_vertex = 0;
  for(uint i = 0; i < g->nr_nodes; ++i) {
    min_counter = g->nr_nodes+1;
    selected_vertex = 0;

    for(uint j = 0; j < g->nr_nodes; ++j) {
      counter = 0;
      if (!used[j]) {
        for(uint k = 0; k < g->nr_nodes; ++k) {
          if (!used[k] && g->is_edge(k, j)) {
            ++counter;
          }
        }

        if (counter < min_counter) {
          min_counter = counter;
          selected_vertex = j;
        }
      }
    }
    order[i] = selected_vertex;
    used[selected_vertex] = true;
  }

  return order;
}

std::vector<uint> reorder_rev(const graph* const g) // revert the order of vertices (usually used to change from small to large)
{
  std::vector<uint> order;
  std::generate_n(std::back_inserter(order), g->nr_nodes, [&](){return g->nr_nodes - order.size() - 1;});
  return order;
}

std::vector<uint> reorder_random(const graph* const g) // permute at random
{
  std::vector<uint> order;
  std::generate_n(std::back_inserter(order), g->nr_nodes, [&](){return order.size();});
  std::random_shuffle(order.begin(), order.end(), [](int i) { return rand()%i; });
  return order;
}

std::vector<uint> reorder_2nb(const graph* const g) // order based on the size of 2-neigborhood, from large to small
{
  // naive but works
  std::vector<std::set<int>> nbs(g->nr_nodes);
  std::vector<uint> order;
  std::vector<std::pair<uint, uint> > degrees; // 2nbs degree

  for (uint i = 0; i < g->nr_nodes; ++i)
    for (uint j = 0; j < g->nr_nodes; ++j)
      if (i != j && g->is_edge(i,j))
        nbs[i].insert(j);
  for(uint i = 0; i < g->nr_nodes; ++i)
    for(uint j = 0; j < g->nr_nodes; ++j)
      if(i != j && g->is_edge(i,j))
        nbs[i].insert(nbs[j].cbegin(), nbs[j].cend()); //union of sets

  std::generate_n(std::back_inserter(degrees), g->nr_nodes, [&](){auto i = degrees.size(); return std::make_pair(nbs[i].size(), i);});
  std::sort(degrees.begin(), degrees.end(), compare_pairs);
  std::generate_n(std::back_inserter(order), degrees.size(), [&](){return degrees[order.size()].second;});
  return order;
}

std::vector<uint> reorder_color(uint s, const graph* const g) // See S. Trukhanov et al.
{
  std::vector<uint> order; // U
  std::vector<uint> col(g->nr_nodes, 0);
  std::vector<uint> colnum(g->nr_nodes, 0);
  uint max_col = 0;
  for(uint i = 0; i < g->nr_nodes; ++i)
  {
    uint u;
    for(u = 0; u < g->nr_nodes && col[u] != 0; ++u);
    std::vector<uint> c;
    uint min_c = g->nr_nodes+1;
    for(uint j = 0; j < g->nr_nodes; ++j)
      if(g->is_edge(u,j) && colnum[j] < s-1)
      {
        c.push_back(col[j]);
        min_c = std::min(min_c, col[j]);
      }

    if(c.size() == 0)
    {
      col[u] = max_col+1;
      max_col++;
    }
    else
    {
      col[i] = min_c;
      max_col = std::max(max_col, min_c);
    }
  }
  for(uint i = 0; i <= max_col; ++i)
  {
    for(uint j = 0; j < g->nr_nodes; ++j)
      if(col[j] == i)
        order.push_back(j);
  }

  return order;
}

std::vector<uint> reorder_weight(const graph* const g) // weight from large to small
{
  std::vector<std::pair<uint,uint> > wt;
  std::vector<uint> order;
  std::generate_n(std::back_inserter(wt), g->nr_nodes, [&](){auto i = wt.size(); return std::make_pair(g->weight(i), i);});
  std::sort(wt.begin(), wt.end(), compare_pairs);
  std::generate_n(std::back_inserter(order), g->nr_nodes, [&](){return wt[order.size()].second;});
  return order;
}

std::vector<uint> reorder_cliquer(const graph* const g) {
  std::vector<unsigned int> order(g->nr_nodes, 0);
  std::vector<int> degree(g->nr_nodes, 0);
  std::vector<int> used(g->nr_nodes, 0);

	for (unsigned int i=0; i < g->nr_nodes; ++i)
		for (unsigned int j=0; j < g->nr_nodes; ++j)
      if (i != j && g->is_edge(i, j))
        ++degree[i];

  int maxdegree = 0, maxvertex = 0;
  unsigned int v = 0;
  bool samecolor = false;
 
  while (v < g->nr_nodes) {
    used = std::vector<int>(g->nr_nodes, 0);
    do {
      maxdegree = -1;
      samecolor = false;

			for (unsigned int i = 0; i < g->nr_nodes; ++i) {
				if (!used[i] && degree[i] > maxdegree) {
					maxvertex = i;
					maxdegree = degree[i];
					samecolor = true;
				}
			}
			if (samecolor == true) {
				order[v]=maxvertex;
				degree[maxvertex]=-1;
				v++;

				for (unsigned int i = 0; i < g->nr_nodes; ++i) {
					if (g->is_edge(maxvertex, i)) {
            used[i] = true;
						degree[i]--;
					}
				}
			}

    } while (samecolor);
  }

  return order;
}
