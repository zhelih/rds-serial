#ifndef _VERIFIER_H
#define _VERIFIER_H

#include "graph.h"
#include <vector>

typedef unsigned int uint;

class verifier
{
  public:
    virtual bool check_pair(graph* g, uint i, uint j) = 0;
    virtual bool check(graph* g, const std::vector<uint>& p, uint n, void* aux) = 0;
    virtual void* prepare_aux(graph*g, const std::vector<uint>& p) = 0;
    virtual void free_aux(graph* g, const std::vector<uint>& p, void* aux) = 0;
};

class clique: public verifier
{
  public:
  bool check_pair(graph* g, uint i, uint j) { return g->is_edge(i,j); }
  bool check(graph* g, const std::vector<uint>& p, uint n, void* _aux)
  {
    for(auto it = p.begin(); it != p.end(); ++it)
      if(!g->is_edge(*it, n))
        return false;
    return true;
  }
  void* prepare_aux(graph*, const std::vector<uint>&) { return 0; }
  void free_aux(graph* g, const std::vector<uint>& p, void* aux) {}
};

class stable: public verifier
{
  public:
  bool check_pair(graph* g, uint i, uint j) { return !g->is_edge(i,j); }
  bool check(graph* g, const std::vector<uint>& p, uint n, void* _aux)
  {
    for(auto it = p.begin(); it != p.end(); ++it)
      if(g->is_edge(*it, n))
        return false;
    return true;
  }
  void* prepare_aux(graph*, const std::vector<uint>&) { return 0; }
  void free_aux(graph* g, const std::vector<uint>& p, void* aux) {}
};

class iuc: public verifier
{
  public:
  bool check_pair(graph* g, uint i, uint j) { return true; }
  bool check(graph* g, const std::vector<uint>& p, uint n, void* _aux)
  {
    for(auto it = p.begin(); it != p.end(); ++it)
      for(auto it2 = next(it); it2 != p.end(); ++it2)
      if(g->is_edge(*it, *it2) + g->is_edge(*it, n) + g->is_edge(*it2, n) == 2)
        return false;
    return true;
  }
  void* prepare_aux(graph*, const std::vector<uint>&) { return 0; }
  void free_aux(graph* g, const std::vector<uint>& p, void* aux) {}
};

class defective_clique: public verifier
{
  private:
  uint s;
  public:
  defective_clique(uint s_) : s(s_) { }
  bool check_pair(graph* g, uint i, uint j) { return (s>0)||(g->is_edge(i,j)); }
  bool check(graph* g, const std::vector<uint>& p, uint n, void* aux)
  {
    uint edges = *static_cast<uint*>(aux);
    uint ps = p.size();
    for(uint i = 0; i < ps; ++i)
      if(g->is_edge(p[i],n))
        edges++;
    return edges >= (ps*(ps+1)/2-s);
  }

  void* prepare_aux(graph* g, const std::vector<uint>& p) {
    // calc number of edges in P
    uint *edges = new uint;
    *edges = 0;
    for(uint i = 0; i < p.size(); ++i)
      for(uint j = i+1; j < p.size(); ++j)
        if(g->is_edge(p[i],p[j]))
          (*edges)++;
    return edges;
  }
  void free_aux(graph* g, const std::vector<uint>& p, void* aux) { uint* i = static_cast<uint*>(aux); delete i; }
};


#endif
