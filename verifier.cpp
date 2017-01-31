#include "verifier.h"

#include <vector>
#include <stack>

// clique
bool clique::check_pair(graph* g, uint i, uint j) const { return g->is_edge(i,j); }
bool clique::check(graph* g, const std::vector<uint>& p, uint n)
{
  for(auto it = p.begin(); it != p.end(); ++it)
    if(!g->is_edge(*it, n))
      return false;
  return true;
}
bool clique::check_solution(graph* g, const std::vector<uint>& res) const
{
  for(uint i = 0; i < res.size(); ++i)
    for(uint j = i+1; j > res.size(); ++j)
      if(!g->is_edge(res[i], res[j]))
        return false;
  return true;
}

// stable
bool stable::check_pair(graph* g, uint i, uint j) const { return !g->is_edge(i,j); }
bool stable::check(graph* g, const std::vector<uint>& p, uint n)
{
  for(auto it = p.begin(); it != p.end(); ++it)
    if(g->is_edge(*it, n))
      return false;
  return true;
}
bool stable::check_solution(graph* g, const std::vector<uint>& res) const
{
  for(uint i = 0; i < res.size(); ++i)
    for(uint j = i+1; j > res.size(); ++j)
      if(g->is_edge(res[i], res[j]))
        return false;
  return true;
}

// iuc
bool iuc::check_pair(graph* g, uint i, uint j) const { return true; }
bool iuc::check(graph* g, const std::vector<uint>& p, uint n)
{
  for(auto it = p.begin(); it != p.end(); ++it)
    for(auto it2 = next(it); it2 != p.end(); ++it2)
    if(g->is_edge(*it, *it2) + g->is_edge(*it, n) + g->is_edge(*it2, n) == 2)
      return false;
  return true;
}
bool iuc::check_solution(graph* g, const std::vector<uint>& res) const {
  for(uint i = 0; i < res.size(); ++i)
    for(uint j = i+1; j < res.size(); ++j)
      for(uint k = j+1; k < res.size(); ++k)
      {
        uint ij = g->is_edge(res[i], res[j]);
        uint ik = g->is_edge(res[i], res[k]);
        uint jk = g->is_edge(res[j], res[k]);
        if(ij + ik + jk == 2)
          return false;
      }
  return true;
}

//s-defective clique
bool defective_clique::check_pair(graph* g, uint i, uint j) const { return (s>0)||(g->is_edge(i,j)); }
bool defective_clique::check(graph* g, const std::vector<uint>& p, uint n)
{
  return nnv + nncnt[level][n] <= s;
}
void defective_clique::init_aux(graph* g, uint i, const std::vector<uint>& c)
{
  level = 0;
  nnv = 0;
  nncnt.resize(g->nr_nodes);
  for(uint it = 0; it < g->nr_nodes; ++it)
    nncnt[it].resize(g->nr_nodes);
  for(uint it = 0; it < g->nr_nodes; ++it)
    for(uint it2 = 0; it2 < g->nr_nodes; ++it2)
      nncnt[it][it2] = 0;

  for(uint it = 0; it < c.size(); ++it)
    nncnt[0][c[it]]=!g->is_edge(c[it], i);
}
void defective_clique::prepare_aux(graph* g, const std::vector<uint>& p, uint j, const std::vector<uint>& c)
{
  nnv += nncnt[level][j];
  level++;
  for(uint i = 0; i < c.size(); ++i)
  {
    nncnt[level][c[i]] = nncnt[level-1][c[i]];
    if(!g->is_edge(c[i],j))
      nncnt[level][c[i]]++;
  }
  nncnt[level][j]=nncnt[level-1][j];
}
void defective_clique::undo_aux(graph* g, const std::vector<uint>& p, uint j, const std::vector<uint>& c)
{
  level--;
  nnv -= nncnt[level][j];
}
void defective_clique::free_aux() { level = 0; }
bool defective_clique::check_solution(graph* g, const std::vector<uint>& res) const
{
  uint edges = 0;
  for(uint i = 0; i < res.size(); ++i)
    for(uint j = i+1; j < res.size(); ++j)
      if(g->is_edge(res[i],res[j]))
        edges++;
  return edges >= (res.size()*(res.size()-1)/2 - s);
}

//s-plex
bool plex::check_pair(graph* g, uint i, uint j) const
{
  if(s == 1)
    return g->is_edge(i,j);
  else
    return true;
}
bool plex::check_solution(graph* g, const std::vector<uint>& res) const
{
  std::vector<uint> degrees(res.size());
  for(uint i = 0; i < res.size(); ++i)
    degrees[i] = 0;
  for(uint i = 0; i < res.size(); ++i)
    for(uint j = 0; j < res.size(); ++j)
      if(i != j && g->is_edge(res[i],res[j]))
        degrees[i]++;
  uint m_degree = degrees[0];
  for(uint i = 1; i < res.size(); ++i)
    if(m_degree > degrees[i])
      m_degree = degrees[i];
  fprintf(stderr, "min_degree = %u\n", m_degree);
  return m_degree >= (res.size() - s);
}

bool plex::check(graph* g, const std::vector<uint>& p, uint n)
{
  if(nncnt[level][n] >= s) // degree check
    return false;
  for(uint i = 0; i < nr_sat; ++i) // SAT connectivity check
    if(!g->is_edge(n, sat[i]))
      return false;
  return true;
}
void plex::init_aux(graph* g, uint i, const std::vector<uint>& c)
{
  level = 0;
  nncnt.resize(g->nr_nodes);
  sat.resize(g->nr_nodes);
  nr_sat = 0;
  for(uint it = 0; it < g->nr_nodes; ++it)
    nncnt[it].resize(g->nr_nodes);
  for(uint it = 0; it < c.size(); ++it)
    for(uint l = 0; l < g->nr_nodes; ++l)
      nncnt[l][it] = 0;
  for(uint it = 0; it < c.size(); ++it)
    nncnt[0][c[it]]=!g->is_edge(c[it], i);
}
void plex::prepare_aux(graph*g, const std::vector<uint>& p, uint j, const std::vector<uint>& c)
{
  nr_sat = 0;
  level++;
  for(uint i = 0; i < p.size(); ++i)
  {
    nncnt[level][p[i]] = nncnt[level-1][p[i]];
    if(!g->is_edge(p[i],j))
    {
      nncnt[level][p[i]]++;
      if(nncnt[level][p[i]] == s-1)
      {
        sat[nr_sat] = p[i]; // push_back
        nr_sat++;
      }
    }
  }

  for(uint i = 0; i < c.size(); ++i)
  {
    nncnt[level][c[i]] = nncnt[level-1][c[i]];
    if(!g->is_edge(c[i],j))
    {
      nncnt[level][c[i]]++;
    }
  }
  nncnt[level][j] = nncnt[level-1][j];
  if(nncnt[level][j] == s-1)
  {
    sat[nr_sat] = j;
    nr_sat++;
  }
}
void plex::undo_aux(graph* g, const std::vector<uint>& p, uint j, const std::vector<uint>& c)
{
  level--;
}
void plex::free_aux() { level = 0; }

//maximum forest subgraph
bool forest::check_pair(graph* g, uint i, uint j) const { return true; }
bool forest::check(graph* g, const std::vector<uint>& p, uint n)
{
  for(uint i = 0; i < p.size(); ++i)
    color[p[i]] = 0;
  stack_size = 0;
  s[stack_size] = n; stack_size++;
  parent[n] = n;
  while(stack_size != 0)
  {
    stack_size--;
    uint v = s[stack_size];
    color[v] = 1;
    if(n != parent[v] && g->is_edge(v, n))
      return false;
    for(uint i = 0; i < p.size(); ++i)
    {
      if(p[i] != parent[v] && g->is_edge(v, p[i]))
      {
        if(color[p[i]] == 1)
        {
          return false;
        } else {
          s[stack_size] = p[i]; ++stack_size;
          parent[p[i]] = v;
        }
      }
    }
  }
  return true;
}
bool forest::check_solution(graph* g, const std::vector<uint>& res) const
{
  std::stack<std::pair<uint,uint> > s;
  std::vector<uint> color(g->nr_nodes);
  for(uint i = 0; i < g->nr_nodes; ++i)
    color[i] = 0;
  for(uint j = 0; j < res.size(); ++j)
    if(color[res[j]] == 0) // undiscovered
    {
      // start BFS from j
      s.push(std::make_pair(res[j], res[j]));
      while(!s.empty())
      {
        std::pair<uint,uint> v = s.top(); s.pop();
        color[v.first] = 1;
        for(uint i = 0; i < res.size(); ++i)
        {
          if(res[i] != v.second && g->is_edge(v.first, res[i]))
          {
            if(color[res[i]] == 1)
              return false;
            else
              s.push(std::make_pair(res[i],v.first));
          }
        }
      }
    }
  return true;
}

void forest::init_aux(graph* g, uint i, const std::vector<uint>& c)
{
  color.resize(g->nr_nodes);
  parent.resize(g->nr_nodes);
  s.resize(g->nr_nodes);
}

//bipartite
bool bipartite::check_pair(graph* g, uint i, uint j) const { return true; }
bool bipartite::check(graph* g, const std::vector<uint>& p, uint n)
{
  for(uint i = 0; i < p.size(); ++i)
    color[p[i]] = 2;
  stack_size = 0;
  s[stack_size] = n; stack_size++;
  color[n] = 1;
  while(stack_size > 0)
  {
    stack_size--;
    uint v = s[stack_size];
//    color[v] = flag + 1; flag = !flag;
    for(uint i = 0; i < p.size(); ++i)
    {
      if(g->is_edge(p[i], n))
        if(color[p[i]] == color[n])
          return false;
      if(g->is_edge(p[i], v))
      {
        if(color[p[i]] == color[v])
          return false;
        if(color[p[i]] == 2)
        {
          color[p[i]] = !color[v];
          s[stack_size] = p[i];
          stack_size++;
        }
      }
    }
  }
  return true;
}

void bipartite::init_aux(graph* g, uint i, const std::vector<uint>& c)
{
  s.resize(g->nr_nodes);
  color.resize(g->nr_nodes);
}
