#ifndef _IUC_H
#define _IUC_H
#include "verifier.hpp"

class IUC: public RegisterVerifier<IUC> {
  public:
    bool check_pair(graph* g, uint i, uint j) const {
      return true;
    }

    bool check(graph* g, const std::vector<uint>& p, uint n) const {
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
