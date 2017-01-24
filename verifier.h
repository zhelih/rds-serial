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
    virtual void prepare_aux(graph*g, const std::vector<uint>& p, uint i, const std::vector<uint>& c, void* prev_aux) { }
    virtual void undo_aux(graph* g, const std::vector<uint>& p, uint i, const std::vector<uint>& c, void* aux) {}

    // free aux info
    virtual void free_aux(void* aux) {}
    // optional checker
    virtual bool check_solution(graph* g, const std::vector<uint>& res) const { return true; }

    verifier() {}
    virtual ~verifier() {}
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
  bool check_solution(graph* g, const std::vector<uint>& res) const {
    for(uint i = 0; i < res.size(); ++i)
      for(uint j = i+1; j > res.size(); ++j)
        if(!g->is_edge(res[i], res[j]))
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
  bool check_solution(graph* g, const std::vector<uint>& res) const {
    for(uint i = 0; i < res.size(); ++i)
      for(uint j = i+1; j > res.size(); ++j)
        if(g->is_edge(res[i], res[j]))
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
  bool check_solution(graph* g, const std::vector<uint>& res) const {
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
};

class defective_clique: public verifier
{
  private:
  uint s;
  public:

  struct t_aux { uint nnv; std::vector<uint> nncnt; };

  defective_clique(uint s_) : s(s_) { }
  bool check_pair(graph* g, uint i, uint j) const { return (s>0)||(g->is_edge(i,j)); }
  bool check(graph* g, const std::vector<uint>& p, uint n, void* aux) const
  {
    t_aux* a = (static_cast<t_aux*>(aux));
    return a->nnv + a->nncnt[n] <= s;
  }

  void* init_aux(graph* g, uint i, const std::vector<uint>& c) {
    t_aux *aux = new t_aux();
    aux->nnv=0;
    aux->nncnt.resize(g->nr_nodes);
    for(uint it = 0; it < c.size(); ++it)
    {
      aux->nncnt[c[it]]=!g->is_edge(c[it], i);
    }
    return aux;
  }
  void prepare_aux(graph* g, const std::vector<uint>& p, uint j, const std::vector<uint>& c, void* aux) {
    t_aux* a = static_cast<t_aux*>(aux);

    a->nnv += a->nncnt[j];
    for(uint i = 0; i < c.size(); ++i)
    {
      if(!g->is_edge(c[i],j))
        a->nncnt[c[i]]++;
    }
  }
  void undo_aux(graph* g, const std::vector<uint>& p, uint j, const std::vector<uint>& c, void* aux) {
    t_aux* a = static_cast<t_aux*>(aux);

    for(uint i = 0; i < c.size(); ++i)
    {
      if(!g->is_edge(c[i],j))
        a->nncnt[c[i]]--;
    }
    a->nnv -= a->nncnt[j];
  }
  void free_aux(void* aux) { t_aux* i = static_cast<t_aux*>(aux); delete i; }

  bool check_solution(graph* g, const std::vector<uint>& res) const
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
