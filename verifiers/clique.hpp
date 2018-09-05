#ifndef _CLIQUE_H
#define _CLIQUE_H
#include "verifier.hpp"

class Clique: public RegisterVerifier<Clique> {
  public:
    bool check_pair(graph* g, uint i, uint j) const {
      return g->is_edge(i,j);
    }

    bool check(graph* g, const std::vector<uint>& p, uint n) const {
      for(auto& v: p)
        if(!g->is_edge(v, n))
         return false;
      return true;
    }

    bool check_solution(graph* g, const std::vector<uint>& res) const {
      for (auto& v: res) {
        for (auto& u: res) {
          if (v != u && !g->is_edge(v, u)) {
            return false;
          }
        }
      }
      return true;
    }

    Clique() {
      name = "Clique";
      description = "Well, it's a clique";
      shortcut = "-c";
    }

    Clique* clone() const {
        return new Clique(*this);
    }
};

#endif
