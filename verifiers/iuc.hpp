#ifndef _IUC_H
#define _IUC_H
#include "verifier.hpp"

class IUC: public RegisterVerifier<IUC> {
  public:
    bool check_pair(uint i, uint j) const {
      return true;
    }

    bool check(const std::vector<uint>& p, uint n) const {
      uint a = p.back();
      for(uint b : p)
      {
        if(a == b)
          continue;
        if(g->is_edge(a,b) + g->is_edge(a,n) + g->is_edge(b,n) == 2)
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
            if(ij + ik + jk == 2)
              return false;
          }
      return true;
    }

    IUC() {
      name = "Independent union of cliques";
      description = "Well, it's an IUC";
      shortcut = "-iuc";
    }

    IUC* clone() const {
        return new IUC(*this);
    }
};

#endif
