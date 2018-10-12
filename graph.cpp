#include "graph.h"
#include <cstdio>
#include <string>
#include <vector>
#include <algorithm>
#include <set>
#include <cstdlib>

using namespace std;

#define FROM_1

#ifndef min
#define min(a,b) (((a)<(b))?(a):(b))
#endif
#ifndef max
#define max(a,b) (((a)>(b))?(a):(b))
#endif

graph* from_dimacs(const char* fname)
{
  FILE* f = fopen(fname, "r");
  if(!f)
  {
    fprintf(stderr, "Cannot open file %s\n", fname);
    return 0;
  }

  char buf[1024]; int nr_nodes = 0, nr_edges = 0;

  // search for "n nodes edges"
  while(NULL != fgets(buf, sizeof(buf), f))
  {
    if(buf[0] == 'p')
    {
      sscanf(buf, "p edge %d %d", &nr_nodes, &nr_edges);
      break;
    }
  }

  if(nr_nodes == 0 && nr_edges == 0)
  {
    fclose(f);
    fprintf(stderr, "Cannot found dimacs metadata in %s\n", fname);
    return 0;
  }

//  fprintf(stderr, "Found metadata in %s : (%d, %d)\n", fname, nr_nodes, nr_edges);

  graph* g = new graph(nr_nodes);

  rewind(f);

  //read the edges
  while(NULL != fgets(buf, sizeof(buf), f))
  {
    if(buf[0] == 'e')
    {
      int from, to; double weight;
      sscanf(buf, "e %d %d %lf", &from, &to, &weight);
#ifdef FROM_1
      from--; to--;
#endif
      g->add_edge(from, to);//, weight);
    }
  }

  fprintf(stderr, "graph %s: %d nodes, %d edges\n", fname, nr_nodes, nr_edges);
  fclose(f);
  return g;
}

graph::graph(uint n)
{
/*
  // compute bit mask
  mask[0] = 1;
  for(uint i = 1; i < CHUNK_SIZE; ++i)
    mask[i] = mask[i-1] << 1;

  mapka.resize(n); // used to restore order
*/
  // create adj matrix
  //adj = new uint*[n];
  adj.resize(n);
  for(uint i = 0; i < n; ++i)
  {
    //adj[i] = new uint[n];
    adj[i].resize(n);
//    mapka[i] = i;
  }
  for(uint i = 0; i < n; ++i)
    for(uint j = 0; j < n; ++j)
      adj[i][j] = false;
  nr_nodes = n;

  weights.resize(n);
  for(uint i = 0; i < n; ++i)
    weights[i] = 1;

  current_order.resize(n);
  for (uint i = 0; i < n; ++i) {
    current_order[i] = i;
  }
}

graph::~graph()
{
/*  for(uint i = 0; i < nr_nodes; ++i)
    delete [] adj[i];
  delete [] adj;*/
}

void graph::add_edge(uint i, uint j)
{
  adj[i][j] = true;
  adj[j][i] = true;
}

/*inline bool graph::is_edge(uint i, uint j)
{
  return adj[i][j/CHUNK_SIZE]&mask[j%CHUNK_SIZE];
}*/

/* Reorders start here */
void graph::apply_order(ordering order, bool reverse) {
  switch (order) {
    case ordering::None:
      break;
    case ordering::Degree:
      this->reorder_degree();
      break;
    case ordering::Degeneracy:
      this->reorder_degeneracy();
      break;
    case ordering::Neighborhood:
      this->reorder_2nb();
      break;
    case ordering::NColor:
      this->reorder_color(2);
      break;
  }
  if (reverse) {
    this->reorder_rev();
  }
}

void graph::reorder_none() {} // don't reorder anything

std::vector<uint> graph::reverse_order(const std::vector<uint>& order) const
{
  std::vector<uint> order_reverse(order.size());
  for (uint p = 0; p < nr_nodes; ++p)
  {
    order_reverse[order[p]] = p;
  }
  return order_reverse;
}

void graph::reorder_custom(const vector<uint>& order)
{
  std::vector<uint> order_reverse = this->reverse_order(order);
  bool* adj_new = (bool*)malloc(sizeof(bool)*nr_nodes*nr_nodes);
  for (uint v = 0; v < nr_nodes; ++v)
    for (uint u = 0; u < nr_nodes; ++u)
      adj_new[order_reverse[v]*nr_nodes + order_reverse[u]] = this->adj[v][u];

  for (uint v = 0; v < nr_nodes; ++v)
    for (uint u = 0; u < nr_nodes; ++u)
      this->adj[v][u] = adj_new[v*nr_nodes + u];

  free(adj_new);

  std::vector<uint> weights_new(nr_nodes);
  for (uint v = 0; v < nr_nodes; ++v)
    weights_new[order_reverse[v]] = this->weights[v];
  this->weights = weights_new;

  std::vector<uint> new_order(nr_nodes);
  for (uint i = 0; i < nr_nodes; ++i)
    new_order[i] = this->current_order[order[i]];
  this->current_order = new_order;
}

/*void graph::reorder_custom(const vector<uint>& order)
{ 
  if(order.size() != nr_nodes)
  {
    fprintf(stderr, "Wrong custom order input : size!\n");
    return;
  }
  // recomputing the whole adjacency matrix
  // slow but who cares? done only ones
  vector<vector<uint> > adj_old(nr_nodes);
  vector<uint> mapka_old(mapka);
  for(uint i = 0; i < nr_nodes; ++i)
  {
    adj_old[i].resize(nr_nodes/CHUNK_SIZE+1);
    mapka[i] = order[mapka_old[i]]; // update mapka
  }
  for(uint i = 0; i < nr_nodes; ++i)
    for(uint j = 0; j < nr_nodes/CHUN	K_SIZE+1; ++j)
    {
      adj_old[i][j] = adj[i][j]; // copy
      adj[i][j] = 0; // nullify
    }
  for(uint i = 0; i < nr_nodes; ++i)
  {
    uint node = order[i];
    if(node >= nr_nodes)
    {
      fprintf(stderr, "Wrong custom order input : node out of bounds!\n");
      return;
    }
  }
  for(uint i = 0; i < nr_nodes; ++i)
  {
    for(uint j = 0; j < nr_nodes; ++j)
    {
      // copy adj
      if(adj_old[i][j/CHUNK_SIZE]&mask[j%CHUNK_SIZE]) // if is_edge
        add_edge(order[i], order[j]);
    }
  }
}
*/

void graph::reorder_degree() // degree order from large to small
{
  vector<pair<uint,uint> > degrees(nr_nodes); // pair degree, node number
  for(uint i = 0; i < nr_nodes; ++i)
    degrees[i] = make_pair(0,i);
  for(uint i = 0; i < nr_nodes; ++i)
  {
    for(uint j = 0; j < nr_nodes; ++j)
      if(i != j && is_edge(i,j))
        degrees[i].first++;
  }
  // sort from large to small based on degree
  sort(degrees.begin(), degrees.end(), [](const pair<uint,uint> &left, const pair<uint,uint> &right) { return left.first > right.first; });
  vector<uint> order(nr_nodes);
  for(uint i = 0; i < nr_nodes; ++i)
    order[i] = degrees[i].second;
  reorder_custom(order);

  fprintf(stderr, "reorder degree done!\n");
}

void graph::reorder_degeneracy() {
  std::vector<bool> used(nr_nodes, false);
  vector<uint> order(nr_nodes);
  uint counter = 0, min_counter = nr_nodes+1, selected_vertex = 0;
  for(uint i = 0; i < nr_nodes; ++i) {
    min_counter = nr_nodes+1;
    selected_vertex = 0;

    for(uint j = 0; j < nr_nodes; ++j) {
      counter = 0;
      if (!used[j]) {
        for(uint k = 0; k < nr_nodes; ++k) {
          if (!used[k] && is_edge(k, j)) {
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
  
  reorder_custom(order);

  fprintf(stderr, "reorder degeneracy done!\n");
}

void graph::reorder_rev() // revert the order of vertices (usually used to change from small to large)
{
  vector<uint> order(nr_nodes);
  for(uint i = 0; i < nr_nodes; ++i)
    order[i] = nr_nodes-i-1;
  reorder_custom(order);
  fprintf(stderr, "reorder revert done!\n");
}


void graph::reorder_random() // permute at random
{
  vector<uint> order(nr_nodes);
  for(uint i = 0; i < nr_nodes; ++i)
    order[i] = i;
  random_shuffle(order.begin(), order.end(), [](int i) { return rand()%i; });
  reorder_custom(order);
  fprintf(stderr, "reorder random done!\n");
}

void graph::reorder_2nb() // order based on the size of 2-neigborhood, from large to small
{
  // naive but works
  vector<set<int> > nbs(nr_nodes);
  for(uint i = 0; i < nr_nodes; ++i)
    for(uint j = 0; j < nr_nodes; ++j)
      if(i != j && is_edge(i,j))
        nbs[i].insert(j);
  for(uint i = 0; i < nr_nodes; ++i)
    for(uint j = 0; j < nr_nodes; ++j)
      if(i != j && is_edge(i,j))
        nbs[i].insert(nbs[j].cbegin(), nbs[j].cend()); //union of sets
  vector<pair<uint, uint> > degrees(nr_nodes); // 2nbs degree
  for(uint i = 0; i < nr_nodes; ++i)
    degrees[i] = make_pair(nbs.size(), i);
  sort(degrees.begin(), degrees.end(), [](const pair<uint,uint> &left, const pair<uint,uint> &right) { return left.first > right.first; });
  vector<uint> order(nr_nodes);
  for(uint i = 0; i < nr_nodes; ++i)
    order[i] = degrees[i].second;
  reorder_custom(order);
  fprintf(stderr, "reorder 2-neighborhood done!\n");
}

void graph::reorder_color(uint s) // See S. Trukhanov et al.
{
  vector<uint> order; // U
  vector<uint> col(nr_nodes);
  vector<uint> colnum(nr_nodes);
  uint max_col = 0;
  for(uint i = 0; i < nr_nodes; ++i)
  {
    col[i] = 0;
    colnum[i] = 0;
  }
  for(uint i = 0; i < nr_nodes; ++i)
  {
    uint u;
    for(u = 0; u < nr_nodes && col[u] != 0; ++u);
    if(u == nr_nodes){
      fprintf(stderr, "logic internal error in ordering? : reorder_color, skipping\n");
      return;
    }
    vector<uint> c;
    uint min_c = nr_nodes+1;
    for(uint j = 0; j < nr_nodes; ++j)
      if(is_edge(u,j) && colnum[j] < s-1)
      {
        c.push_back(col[j]);
        min_c = min(min_c, col[j]);
      }

    if(c.size() == 0)
    {
      col[u] = max_col+1;
      max_col++;
    }
    else
    {
      col[i] = min_c;
      max_col = max(max_col, min_c);
    }
  }
  for(uint i = 0; i <= max_col; ++i)
  {
    for(uint j = 0; j < nr_nodes; ++j)
      if(col[j] == i)
        order.push_back(j);
  }
  if(order.size() != nr_nodes)
  {
    fprintf(stderr, "Internal error: reorder_color, skipping\n");
    return;
  }
  reorder_custom(order);
}

void graph::reorder_weight() // weight from large to small
{
  vector<pair<uint,uint> > wt;
  vector<uint> order(nr_nodes);
  for(uint i = 0; i < nr_nodes; ++i)
    wt.push_back(make_pair(weight(i), i));
  sort(wt.begin(), wt.end(), [](const pair<uint,uint> &left, const pair<uint,uint> &right) { return left.first > right.first; });
  for(uint i = 0; i < nr_nodes; ++i)
    order[i] = wt[i].second;
  reorder_custom(order);
}

void graph::restore_order(vector<uint>& v)
{
  for (auto&& i: v)
  {
    i = current_order[i];
  }
}

void graph::read_weights(const char* filename)
{
  fprintf(stderr, "Reading weights from %s.\nNB!!! Currently vertex ordering is not supported for weights!\n", filename);
  FILE *f;
  f = fopen(filename, "r");
  if(!f)
  {
    fprintf(stderr, "Cannot open %s\n", filename);
    exit(1);
  }
  int v;
  for(uint i = 0; i < nr_nodes; ++i)
    v = fscanf(f, "%u ", &weights[i]);
  if (v == !v) goto PANIC;
  PANIC:
  fclose(f);
}

graph* graph::complement() const
{
  graph* g = new graph(nr_nodes);
  for(unsigned int i = 0; i < nr_nodes; ++i)
    for(unsigned int j = i+1; j < nr_nodes; ++j)
      if(!adj[i][j])
        g->add_edge(i,j);
  return g;
}
