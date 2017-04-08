#include "graph.h"
#include <cstdio>
#include <string>
#include <vector>
#include <algorithm>
#include <set>

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

  printf("graph: %d nodes, %d edges\n", nr_nodes, nr_edges);

  fclose(f);
  return g;
}

graph::graph(uint n)
{
  // compute bit mask
  mask[0] = 1;
  for(uint i = 1; i < CHUNK_SIZE; ++i)
    mask[i] = mask[i-1] << 1;

  mapka.resize(n); // used to restore order
  // create adj matrix
  //adj = new uint*[n];
  adj.resize(n);
  for(uint i = 0; i < n; ++i)
  {
    //adj[i] = new uint[n];
    adj[i].resize(n);
    mapka[i] = i;
  }
  for(uint i = 0; i < n; ++i)
    for(uint j = 0; j < n; ++j)
      adj[i][j] = 0;
  nr_nodes = n;

  weights.resize(n);
  for(uint i = 0; i < n; ++i)
    weights[i] = 1;
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
void graph::reorder_none() {} // don't reorder anything
void graph::reorder_custom(const vector<uint>& order)
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
    for(uint j = 0; j < nr_nodes/CHUNK_SIZE+1; ++j)
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
  sort(degrees.begin(), degrees.end(), [](auto &left, auto &right) { return left.first > right.first; });
  vector<uint> order(nr_nodes);
  for(uint i = 0; i < nr_nodes; ++i)
    order[i] = degrees[i].second;
  reorder_custom(order);

  printf("reorder degree done!\n");
}

void graph::reorder_rev() // revert the order of vertices (usually used to change from small to large)
{
  vector<uint> order(nr_nodes);
  for(uint i = 0; i < nr_nodes; ++i)
    order[i] = nr_nodes-i-1;
  reorder_custom(order);
  printf("reorder revert done!\n");
}


void graph::reorder_random() // permute at random
{
  vector<uint> order(nr_nodes);
  for(uint i = 0; i < nr_nodes; ++i)
    order[i] = i;
  random_shuffle(order.begin(), order.end(), [](int i) { return rand()%i; });
  reorder_custom(order);
  printf("reorder random done!\n");
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
  sort(degrees.begin(), degrees.end(), [](auto &left, auto &right) { return left.first > right.first; });
  vector<uint> order(nr_nodes);
  for(uint i = 0; i < nr_nodes; ++i)
    order[i] = degrees[i].second;
  reorder_custom(order);
  printf("reorder 2-neighborhood done!\n");
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
    uint min_c = -1;
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
  sort(wt.begin(), wt.end(), [](auto &left, auto &right) { return left.first > right.first; });
  for(uint i = 0; i < nr_nodes; ++i)
    order[i] = wt[i].second;
  reorder_custom(order);
}

void graph::restore_order(vector<uint>& v)
{
  vector<uint> rev_mapka(nr_nodes);
  vector<uint> v_copy(v);
  for(uint i = 0; i < nr_nodes; ++i)
    rev_mapka[mapka[i]] = i;
  for(uint i = 0; i < v.size(); ++i)
    v[i] = rev_mapka[v_copy[i]];
}

void graph::print_mapka() const
{
  printf("mapka:\n");
  for(uint i = 0; i < nr_nodes; ++i)
    printf("%u ", mapka[i]);
  printf("\n");
}

void graph::read_weights(const char* filename)
{
  printf("Reading weights from %s.\nNB!!! Currently vertex ordering is not supported for weights!\n", filename);
  FILE *f;
  f = fopen(filename, "r");
  if(!f)
  {
    fprintf(stderr, "Cannot open %s\n", filename);
    exit(1);
  }
  for(uint i = 0; i < nr_nodes; ++i)
    fscanf(f, "%u ", &weights[i]);
  fclose(f);
}
