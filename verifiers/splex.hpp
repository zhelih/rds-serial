#ifndef _SPLEX_H
#define _SPLEX_H
#include "verifier.hpp"
#include <algorithm>

class SPlex: public RegisterVerifier<SPlex> {
  private:
    uint s, nr_sat;
    std::vector<uint> sat;
    std::vector<uint> nncnt;

  public:
    inline bool check_pair(uint i, uint j) const {
      if(s == 1)
        return g->is_edge(i,j);
      else
        return true;
    }

    inline bool check(const std::vector<uint>& p, uint _, uint n) const {
      if(nncnt[n] >= s) // degree check
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
      nr_sat = 0;
      nncnt.resize(g->nr_nodes);
      sat.resize(g->nr_nodes);

      nncnt[i] = 0;
      for(uint v: c)
        nncnt[v]=!g->is_edge(v, i);
    }

    void prepare_aux(const std::vector<uint>& p, uint j, const std::vector<uint>& c, uint c_start)
    {
      nr_sat = 0;
      for(uint v: p) {
        if (!g->is_edge(v, j)) {
          ++nncnt[v];
          if(nncnt[v] == s-1) {
            sat[nr_sat] = v;
            ++nr_sat;
          }
        }
      }

      for(uint it = c_start; it < c.size(); ++it) {
        uint v = c[it];
        if(!g->is_edge(v,j))
          ++nncnt[v];
      }
      if(nncnt[j] == s-1)
      {
        sat[nr_sat] = j;
        ++nr_sat;
      }
    }

    void undo_aux(const std::vector<uint>& p, uint j, const std::vector<uint>& c, uint c_start)
    {
      for(uint v: p) {
        if(!g->is_edge(v, j))
          --nncnt[v];
      }
      for(uint it = c_start; it < c.size(); ++it) {
        uint v = c[it];
        if(!g->is_edge(v,j))
          --nncnt[v];
      }
    }

    void free_aux() {
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
