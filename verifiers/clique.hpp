#ifndef _CLIQUE_H
#define _CLIQUE_H
#include "verifier.hpp"

class Clique: public RegisterVerifier<Clique> {
  public:
    inline bool check_pair(uint i, uint j) const {
      return g->is_edge(i,j);
    }

    inline bool check(const std::vector<uint>& p, uint i, uint n) const {
      return g->is_edge(n, i);
    }

    NO_AUX

    inline bool check_solution(const std::vector<uint>& res) const {
      for (uint v: res) {
        for (uint u: res) {
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
