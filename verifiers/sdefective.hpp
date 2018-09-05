#ifndef _SDEFECTIVE_H
#define _SDEFECTIVE_H
#include "verifier.hpp"

class SDefective: public RegisterVerifier<SDefective> {
  private:
    uint s, level, nnv;
    std::vector<std::vector<uint>> nncnt;

  public:
    bool check_pair(graph* g, uint i, uint j) const {
      return (s>0)||(g->is_edge(i,j));
    }

    bool check(graph* g, const std::vector<uint>& p, uint n) const {
      return nnv + nncnt[level][n] <= s;
    }

    bool check_solution(graph* g, const std::vector<uint>& res) const {
      uint edges = 0;
      for(uint i = 0; i < res.size(); ++i)
        for(uint j = i+1; j < res.size(); ++j)
          if(g->is_edge(res[i],res[j]))
            edges++;
      return edges >= (res.size()*(res.size()-1)/2 - s);
    }

    void init_aux(graph *g, uint i, const std::vector<uint>& c) {
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

    void prepare_aux(graph* g, const std::vector<uint>& p, uint j, const std::vector<uint>& c)
    {
      nnv += nncnt[level][j];
      level++;
      for(auto& v: c) {
        nncnt[level][v] = nncnt[level-1][v];
        if(!g->is_edge(v, j))
          nncnt[level][v]++;
      }
      nncnt[level][j]=nncnt[level-1][j];
    }

    void undo_aux(graph* g, const std::vector<uint>& p, uint j, const std::vector<uint>& c)
    {
      level--;
      nnv -= nncnt[level][j];
    }

    void free_aux() {
      level = 0;
    }

    SDefective() {
      name = "s-Defective clique";
      description = "Well, it's an SDC";
      shortcut = "-sd";
    }

    SDefective* clone() const {
        return new SDefective(*this);
    }
};

#endif
