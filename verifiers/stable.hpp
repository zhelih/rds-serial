#ifndef _STABLE_H
#define _STABLE_H
#include "verifier.hpp"

class Stable: public RegisterVerifier<Stable> {
  public:
    inline bool check_pair(uint i, uint j) const {
      return (!g->is_edge(i,j));
    }

    inline bool check(const std::vector<uint>& p, uint i, uint n) const
    {
      return (!g->is_edge(n, i));
    }

    bool check_solution(const std::vector<uint>& res) const {
      for (auto& v: res) {
        for (auto& u: res) {
          if (v != u && g->is_edge(v, u)) {
            return false;
          }
        }
      }
      return true;
    }

    Stable() {
      name = "Stable set";
      description = "Well, it's a stable set";
      shortcut = "-s";
    }

    Stable* clone() const {
        return new Stable(*this);
    }
};

#endif
