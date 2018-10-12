#ifndef _CLIQUE_H
#define _CLIQUE_H
#include "verifier.hpp"

class Clique: public RegisterVerifier<Clique> {
  public:
    bool check_pair(uint i, uint j) const {
      return g->is_edge(i,j);
    }

    bool check(const std::vector<uint>& p, uint n) const {
<<<<<<< HEAD
      for(auto& v: p)
        if(!g->is_edge(v, n))
         return false;
=======
      uint v = p.back();
      if(!g->is_edge(v, n))
        return false;
>>>>>>> 21f67137b4556e55689bb29c5a16577561456a0e
      return true;
    }

    bool check_solution(const std::vector<uint>& res) const {
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
