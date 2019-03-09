#ifndef _SDEFECTIVE_H
#define _SDEFECTIVE_H
#include "verifier.hpp"

class SDefective: public RegisterVerifier<SDefective> {
  private:
    uint s, nnv;
    std::vector<uint> nncnt;

  public:
    inline bool check_pair(uint i, uint j) const {
      return (s>0)||(g->is_edge(i,j));
    }

    inline bool check(const std::vector<uint>& p, uint i, uint n) const {
      return nnv + nncnt[n] <= s;
    }

    bool check_solution(const std::vector<uint>& res) const {
      uint edges = 0;
      for(uint i = 0; i < res.size(); ++i)
        for(uint j = i+1; j < res.size(); ++j)
          if(g->is_edge(res[i],res[j]))
            edges++;
      return edges >= (res.size()*(res.size()-1)/2 - s);
    }

    void init_aux(uint i, const std::vector<uint>& c) {
      nnv = 0;
      nncnt.resize(g->nr_nodes);
      nncnt[i] = 0;
      for(uint it = 0; it < c.size(); ++it)
        nncnt[c[it]]=!g->is_edge(c[it], i);
    }

    void prepare_aux(const std::vector<uint>& p, uint j, const std::vector<uint>& c, uint c_start)
    {
      nnv += nncnt[j];
      for(uint it = c_start; it < c.size(); ++it) {
        uint v = c[it];
        if(!g->is_edge(v, j))
          nncnt[v]++;
      }
    }

    void undo_aux(const std::vector<uint>& p, uint j, const std::vector<uint>& c, uint c_start)
    {
      for(uint it = c_start; it < c.size(); ++it) {
        uint v = c[it];
        if(!g->is_edge(v, j))
          nncnt[v]--;
      }
      nnv -= nncnt[j];
    }

    void free_aux() {
    }

    SDefective() {
      name = "s-Defective clique";
      description = "Well, it's an SDC";
      shortcut = "-sd";
      add_parameter("s", &s);
    }

    SDefective* clone() const {
        return new SDefective(*this);
    }
};

#endif
