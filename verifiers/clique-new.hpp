#ifndef _CLIQUE_NEW_H
#define _CLIQUE_NEW_H
#include "verifier.hpp"
#include <deque>

class CliqueNew: public RegisterVerifier<CliqueNew> {
  std::deque<unsigned int> last_added;

  public:
    bool check_pair(uint i, uint j) const {
      return g->is_edge(i,j);
    }

    bool check(const std::vector<uint>& p, uint n) const {
      if (last_added.empty()) return true;
      return g->is_edge(n, last_added.back());
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

    void init_aux(uint i, const std::vector<uint>& c) {
    }

    void prepare_aux(const std::vector<uint>& p, uint j, const std::vector<uint>& c)
    {
      last_added.push_back(j);
    }

    void undo_aux(const std::vector<uint>& p, uint j, const std::vector<uint>& c)
    {
      last_added.pop_back();
    }

    void free_aux() {
    }

    CliqueNew() {
      name = "Clique";
      description = "Well, it's a clique";
      shortcut = "-cn";
    }

    CliqueNew* clone() const {
        return new CliqueNew(*this);
    }
};

#endif
