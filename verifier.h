#ifndef _VERIFIER_H
#define _VERIFIER_H

#include "graph.h"
#include <vector>
#include <map>

typedef unsigned int uint;

class verifier
{
  public:
    virtual bool check_pair(graph* g, uint i, uint j) const = 0;
    virtual bool check(graph* g, const std::vector<uint>& p, uint n, void* aux) const = 0;

    // return aux info for singleton P = { i } and C
    virtual void* init_aux(graph* g, uint i, const std::vector<uint>& c) { return 0; }
    // return aux info for P u {i}
    // knowing aux for P as prev_aux
    virtual void* prepare_aux(graph*g, const std::vector<uint>& p, uint i, const std::vector<uint>& c, void* prev_aux) { return 0; }
    // free aux info
    virtual void free_aux(void* aux) {}
};

class clique: public verifier
{
  public:
  bool check_pair(graph* g, uint i, uint j) const { return g->is_edge(i,j); }
  bool check(graph* g, const std::vector<uint>& p, uint n, void* _aux) const
  {
    for(auto it = p.begin(); it != p.end(); ++it)
      if(!g->is_edge(*it, n))
        return false;
    return true;
  }
};

class stable: public verifier
{
  public:
  bool check_pair(graph* g, uint i, uint j) const { return !g->is_edge(i,j); }
  bool check(graph* g, const std::vector<uint>& p, uint n, void* _aux) const
  {
    for(auto it = p.begin(); it != p.end(); ++it)
      if(g->is_edge(*it, n))
        return false;
    return true;
  }
};

class iuc: public verifier
{
  public:
  bool check_pair(graph* g, uint i, uint j) const { return true; }
  bool check(graph* g, const std::vector<uint>& p, uint n, void* _aux) const
  {
    for(auto it = p.begin(); it != p.end(); ++it)
      for(auto it2 = next(it); it2 != p.end(); ++it2)
      if(g->is_edge(*it, *it2) + g->is_edge(*it, n) + g->is_edge(*it2, n) == 2)
        return false;
    return true;
  }
};

class defective_clique: public verifier
{
  private:
  uint s;
  public:

  struct t_aux { uint nnv; std::vector<uint> nncnt; std::vector<uint> i_nncnt;};

  defective_clique(uint s_) : s(s_) { }
  bool check_pair(graph* g, uint i, uint j) const { return (s>0)||(g->is_edge(i,j)); }
  bool check(graph* g, const std::vector<uint>& p, uint n, void* aux) const
  {
    t_aux* a = (static_cast<t_aux*>(aux));
    return a->nnv + a->nncnt[a->i_nncnt[n]] <= s;
  }

  void* init_aux(graph* g, uint i, const std::vector<uint>& c) {
    t_aux *aux = new t_aux();
    aux->nnv=0;
    aux->nncnt.resize(c.size()); aux->i_nncnt.resize(g->nr_nodes);
    for(uint it = 0; it < c.size(); ++it)
    {
      aux->nncnt[it]=!g->is_edge(c[it], i);
      aux->i_nncnt[c[it]] = it;
    }
    return aux;
  }
  void* prepare_aux(graph* g, const std::vector<uint>& p, uint j, const std::vector<uint>& c, void* prev_aux) {
    t_aux* prev = static_cast<t_aux*>(prev_aux);
    t_aux* new_t = new t_aux();

    new_t->nnv = prev->nnv + prev->nncnt[prev->i_nncnt[j]];
    new_t->i_nncnt.resize(g->nr_nodes);
    new_t->nncnt.resize(c.size());

    for(uint i = 0; i < c.size(); ++i)
    {
      new_t->nncnt[i] = prev->nncnt[prev->i_nncnt[c[i]]];
      new_t->i_nncnt[c[i]] = i;
      if(!g->is_edge(c[i],j))
        new_t->nncnt[i]++;
    }

    return new_t;
  }
  void free_aux(void* aux) { t_aux* i = static_cast<t_aux*>(aux); delete i; }

  bool check_solution(graph* g, const std::vector<uint>& res)
  {
    uint edges = 0;
    for(uint i = 0; i < res.size(); ++i)
      for(uint j = i+1; j < res.size(); ++j)
        if(g->is_edge(res[i],res[j]))
          edges++;
    return edges >= (res.size()*(res.size()-1)/2 - s);
  }
};


#endif
