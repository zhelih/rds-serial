#ifndef _SPLEX_H
#define _SPLEX_H
#include "verifier.hpp"
#include <algorithm>

class SPlex: public RegisterVerifier<SPlex> {
  private:
    uint s, level, nr_sat;
    std::vector<uint> sat;
    std::vector<std::vector<uint>> nncnt;

  public:
    inline bool check_pair(uint i, uint j) const {
      if(s == 1)
        return g->is_edge(i,j);
      else
        return true;
    }

    inline bool check(const std::vector<uint>& p, uint _, uint n) const {
      if(nncnt[level][n] >= s) // degree check
        return false;
      for(uint i = 0; i < nr_sat; ++i) // SAT connectivity check
        if(!g->is_edge(n, sat[i]))
          return false;
      return true;
    }

    bool check_solution(const std::vector<uint>& res) const {
      if(res.size() == 0)
        return true;
      std::vector<uint> degrees(res.size());
      for(uint i = 0; i < res.size(); ++i)
        degrees[i] = 0;
      for(uint i = 0; i < res.size(); ++i){
        uint v = res[i];
        for(uint u: res)
          if (v != u && g->is_edge(v, u))
            ++degrees[i];
      }
      uint m_degree = *std::min_element(degrees.begin(), degrees.end());
      return m_degree >= (res.size() - s);
    }

    void init_aux(uint i, const std::vector<uint>& c) {
      level = 0;
      nr_sat = 0;
/*      for(uint it = 0; it < c.size(); ++it)
        for(uint l = 0; l < g->nr_nodes; ++l)
          nncnt[l][it] = 0;*/
      nncnt.resize(g->nr_nodes);
      sat.resize(g->nr_nodes);
      for(uint it = 0; it < g->nr_nodes; ++it)
        nncnt[it].resize(g->nr_nodes);

      nncnt[0][i] = 0;
      for(uint v: c)
        nncnt[0][v]=!g->is_edge(v, i);
    }

    void prepare_aux(const std::vector<uint>& p, uint j, const std::vector<uint>& c, uint c_start)
    {
      nr_sat = 0;
      //level++;
      for(uint v: p) {
        //nncnt[level][v] = nncnt[level-1][v];
        if (!g->is_edge(v, j)) {
          ++nncnt[0][v];
          if(nncnt[0][v] == s-1) {
            sat[nr_sat] = v;
            ++nr_sat;
          }
        }
      }

      for(uint it = c_start; it < c.size(); ++it) {
        uint v = c[it];
        //nncnt[0][v] = nncnt[level-1][v];
        if(!g->is_edge(v,j))
          ++nncnt[0][v];
      }
      //nncnt[level][j] = nncnt[level-1][j];
      if(nncnt[0][j] == s-1)
      {
        sat[nr_sat] = j;
        ++nr_sat;
      }
    }

    void undo_aux(const std::vector<uint>& p, uint j, const std::vector<uint>& c, uint c_start)
    {
      //--level;
      for(uint v: p) {
        if(!g->is_edge(v, j))
          --nncnt[0][v];
      }
      for(uint it = c_start; it < c.size(); ++it) {
        uint v = c[it];
        if(!g->is_edge(v,j))
          --nncnt[0][v];
      }
    }

    void free_aux() {
      level = 0;
      nr_sat = 0;
    }

    SPlex() {
      name = "s-Plex";
      description = "Well, it's an s-Plex";
      shortcut = "-sp";
      add_parameter("s", &s);
    }

    SPlex* clone() const {
        return new SPlex(*this);
    }
};

#endif
