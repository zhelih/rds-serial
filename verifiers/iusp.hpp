#ifndef _IUSP_H
#define _IUSP_H
#include "verifier.hpp"
#include <vector>
#include <unordered_set>
#include <iostream>

class IUSP: public RegisterVerifier<IUSP> {
  private:
    uint s, level;
    std::vector<std::vector<std::unordered_set<uint>>> splexes;

  public:
    inline bool check_pair(uint i, uint j) const {
      return true;
    }

    inline bool check(const std::vector<uint>& p, uint i, uint n) const {
      std::unordered_set<uint> newsplex = join(split(n).first);
      newsplex.insert(n);
      int cnt = 0;
      for (auto& V: newsplex) {
        for (auto& U: newsplex) {
          if (g->is_edge(V, U)) ++cnt;
        }
        if (cnt < static_cast<int>(newsplex.size()) - 1 - static_cast<int>(s)) {
          return false;
        }
      }
      return true;
    }

    void init_aux(uint i, const std::vector<uint>& c) {
      level = 0;
      splexes.resize(g->nr_nodes);
      std::unordered_set<uint> initial;
      initial.insert(i);
      splexes[level].push_back(initial);
    }

    std::pair<std::vector<std::unordered_set<uint>>, std::vector<std::unordered_set<uint>>> split(uint V) const {
      auto& splex = splexes[level];
      std::pair<std::vector<std::unordered_set<uint>>, std::vector<std::unordered_set<uint>>> result;
      for (auto& sdef: splex) {
        bool found = false;
        for (auto& v: sdef) {
          if (g->is_edge(V, v)) {
            result.first.push_back(sdef);
            found = true;
            break;
          }
        }
        if (!found) {
          result.second.push_back(sdef);
        }
      }
      return result;
    }

    std::unordered_set<uint> join(const std::vector<std::unordered_set<uint>>& source) const {
      std::unordered_set<uint> result;
      for (auto& set: source) {
        for (auto& element: set) {
          result.insert(element);
        }
      }
      return result;
    }

    void prepare_aux(const std::vector<uint>& p, uint j, const std::vector<uint>& c)
    {
      std::pair<std::vector<std::unordered_set<uint>>, std::vector<std::unordered_set<uint>>> parts = split(j);
      splexes[++level] = parts.second;
      std::unordered_set<uint> newsdef = join(parts.first);
      newsdef.insert(j);
      splexes[level].push_back(newsdef);
    }

    void undo_aux(const std::vector<uint>& p, uint j, const std::vector<uint>& c)
    {
      splexes[level--].clear();
    }

    void free_aux() {
      splexes.clear();
      level = 0;
    }

    bool check_solution(const std::vector<uint>& res) const {
      return true;
    }

    IUSP() {
      name = "Independent union of cliques";
      description = "Well, it's an IUSP";
      shortcut = "-iusp";
      add_parameter("s", &s);
    }

    IUSP* clone() const {
        return new IUSP(*this);
    }
};

#endif
