#ifndef _STABLE_H
#define _STABLE_H
#include "verifier.hpp"

class Stable: public RegisterVerifier<Stable> {
  public:
    bool check_pair(graph* g, uint i, uint j) const {
      return !g->is_edge(i,j);
    }

    bool check(graph* g, const std::vector<uint>& p, uint n) const
    {
      for(auto& v: p) {
        if (g->is_edge(v, n)) {
          return false;
        }
      }
      return true;
    }

    bool check_solution(graph* g, const std::vector<uint>& res) const {
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
        return new Stable();
    }
};

#endif
