#ifndef _IUSD_H
#define _IUSD_H
#include "verifier.hpp"
#include <vector>
#include <unordered_set>
#include <iostream>

class IUSD: public RegisterVerifier<IUSD> {
  private:
    uint s, level;
    std::vector<std::vector<std::unordered_set<uint>>> sdefectives;

  public:
    inline bool check_pair(uint i, uint j) const {
      return true;
    }

    inline bool check(const std::vector<uint>& p, uint i, uint n) const {
      std::unordered_set<uint> newsdef = join(split(n).first);
      newsdef.insert(n);
      int cnt = 0;
      for (auto& V: newsdef) {
        for (auto& U: newsdef) {
          if (U > V && g->is_edge(V, U)) ++cnt;
        }
      }

      int N = newsdef.size();
      int S = s;
      if (cnt >= (N*(N-1)/2 - S)) {
        return true;
      }
      else {
        return false;
      }
    }

    void init_aux(uint i, const std::vector<uint>& c) {
      level = 0;
      sdefectives.resize(g->nr_nodes);
      std::unordered_set<uint> initial;
      initial.insert(i);
      sdefectives[level].push_back(initial);
    }

    std::pair<std::vector<std::unordered_set<uint>>, std::vector<std::unordered_set<uint>>> split(uint V) const {
      auto& sdefective = sdefectives[level];
      std::pair<std::vector<std::unordered_set<uint>>, std::vector<std::unordered_set<uint>>> result;
      for (auto& sdef: sdefective) {
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
      sdefectives[++level] = parts.second;
      std::unordered_set<uint> newsdef = join(parts.first);
      newsdef.insert(j);
      sdefectives[level].push_back(newsdef);
    }

    void undo_aux(const std::vector<uint>& p, uint j, const std::vector<uint>& c)
    {
      sdefectives[level--].clear();
    }

    void free_aux() {
      sdefectives.clear();
      level = 0;
    }

    bool check_solution(const std::vector<uint>& res) const {
      return true;
    }

    IUSD() {
      name = "Independent union of cliques";
      description = "Well, it's an IUSD";
      shortcut = "-iusd";
      add_parameter("s", &s);
    }

    IUSD* clone() const {
        return new IUSD(*this);
    }
};

#endif
