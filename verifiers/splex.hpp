#ifndef _SPLEX_H
#define _SPLEX_H
#include "verifier.hpp"
#include <algorithm>

class SPlex: public RegisterVerifier<SPlex> {
  private:
    uint s, level, nr_sat;
    std::vector<uint> sat;
    std::vector<std::vector<uint>> nncnt;;

  public:
    bool check_pair(graph* g, uint i, uint j) const {
      if(s == 1)
        return g->is_edge(i,j);
      else
        return true;
    }

    bool check(graph* g, const std::vector<uint>& p, uint n) const {
      if(nncnt[level][n] >= s) // degree check
        return false;
      for(uint i = 0; i < nr_sat; ++i) // SAT connectivity check
        if(!g->is_edge(n, sat[i]))
          return false;
      return true;
    }

    bool check_solution(graph* g, const std::vector<uint>& res) const {
      std::vector<uint> degrees(res.size());
      for(uint i = 0; i < res.size(); ++i)
        degrees[i] = 0;
      for(uint i = 0; i < res.size(); ++i){
        uint v = res[i];
        for(auto& u: res)
          if (v != u && g->is_edge(v, u))
            ++degrees[i];
      }
      uint m_degree = *std::max_element(degrees.begin(), degrees.end());
      return m_degree >= (res.size() - s);
    }

    void init_aux(graph *g, uint i, const std::vector<uint>& c) {
      level = 0;
      nncnt.resize(g->nr_nodes);
      sat.resize(g->nr_nodes);
      nr_sat = 0;
      for(uint it = 0; it < g->nr_nodes; ++it)
        nncnt[it].resize(g->nr_nodes);
      for(uint it = 0; it < c.size(); ++it)
        for(uint l = 0; l < g->nr_nodes; ++l)
          nncnt[l][it] = 0;
      for(auto& v: c)
        nncnt[0][v]=!g->is_edge(v, i);
    }

    void prepare_aux(graph* g, const std::vector<uint>& p, uint j, const std::vector<uint>& c)
    {
      nr_sat = 0;
      level++;
      for(auto& v: p) {
        nncnt[level][v] = nncnt[level-1][v];
        if (!g->is_edge(v, j)) {
          ++nncnt[level][v];
          if(nncnt[level][v] == s-1) {
            sat[nr_sat] = v;
            ++nr_sat;
          }
        }
      }

      for(auto& v: c) {
        nncnt[level][v] = nncnt[level-1][v];
        if(!g->is_edge(v,j))
          nncnt[level][v]++;
      }
      nncnt[level][j] = nncnt[level-1][j];
      if(nncnt[level][j] == s-1)
      {
        sat[nr_sat] = j;
        nr_sat++;
      }
    }

    void undo_aux(graph* g, const std::vector<uint>& p, uint j, const std::vector<uint>& c)
    {
      level--;
    }

    void free_aux() {
      level = 0;
    }

    SPlex() {
      name = "s-Plex";
      description = "Well, it's an s-Plex";
      shortcut = "-sp";
    }

    SPlex* clone() const {
        return new SPlex(*this);
    }
};

#endif
