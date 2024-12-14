#ifndef _dep1_H
#define _dep1_H
#include "verifier.hpp"

class dep1: public RegisterVerifier<dep1> {
  public:
    inline bool check_pair(uint i, uint j) const {
      return true;
    }

    inline bool check(const std::vector<uint>& p, uint a, uint n) const {
      for(uint b : p)
      {
        if(a == b)
          continue;
        if(g->is_edge(a,b) + g->is_edge(a,n) + g->is_edge(b,n) == 2 ||
           g->is_edge(a,b) + g->is_edge(a,n) + g->is_edge(b,n) == 3)
          return false;
      }
      return true;
    }

    bool check_solution(const std::vector<uint>& res) const {
      for(uint i = 0; i < res.size(); ++i)
        for(uint j = i+1; j < res.size(); ++j)
          for(uint k = j+1; k < res.size(); ++k)
          {
            uint ij = g->is_edge(res[i], res[j]);
            uint ik = g->is_edge(res[i], res[k]);
            uint jk = g->is_edge(res[j], res[k]);
            if(ij + ik + jk == 2 || ij + ik + jk == 3)
              return false;
          }
      std::vector<uint> degrees(res.size());
      for(uint i = 0; i < res.size(); ++i)
        degrees[i] = 0;
      for(uint i = 0; i < res.size(); ++i){
        uint v = res[i];
        for(uint u: res)
          if (v != u && g->is_edge(v, u))
            ++degrees[i];
      }
      uint m_degree = *std::max_element(degrees.begin(), degrees.end());
      return m_degree <= 1;
    }

    dep1() {
      name = "1-dependent set";
      description = "Well, it's a 1-dependent set";
      shortcut = "-dep1";
    }

    dep1* clone() const {
        return new dep1(*this);
    }

    NO_AUX;
};

#endif
