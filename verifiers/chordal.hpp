#ifndef _CHORDALNEW_H
#define _CHORDALNEW_H
#include "verifier.hpp"
#include <deque>
#include <algorithm>
#include <iterator>
#include <set>

class ChordalNew: public RegisterVerifier<ChordalNew> {
  std::vector<std::vector<uint>> colors;
  std::vector<uint> colors_to_update;

  uint level = 0;

  public:
    bool check_pair(uint i, uint j) const {
      return true;
    }

    inline bool check(const std::vector<uint>& p, uint i, uint n) const {
      std::vector<uint> notnbh;
      std::vector<uint> nbh;
      std::copy_if(p.begin(), p.end(), std::back_inserter(notnbh), [&](uint v){return !g->is_edge(n, v);});
      std::copy_if(p.begin(), p.end(), std::back_inserter(nbh), [&](uint v){return g->is_edge(n, v);});

      std::deque<uint> bfs;
      std::vector<int> reachable(g->nr_nodes, -1);
      for (uint v: nbh) {
        bfs.push_back(v);
        while (!bfs.empty()) {
          uint z = bfs.front();
          bfs.pop_front();

          for (uint u: notnbh) {
            if (reachable[u] != -1 && reachable[u] != static_cast<int>(v)) return false;
            if (g->is_edge(z, u) && reachable[u] == -1) {
              bfs.push_back(u);
              reachable[u] = v;
            }
          }
        }
      }
      return true;
    }

    bool is_symplical(const uint v, const std::set<unsigned int>& vertices) const {
      std::vector<uint> nbh;
      std::copy_if(vertices.begin(), vertices.end(), std::back_inserter(nbh), [&](uint u){return g->is_edge(v, u);});
      for (uint u: nbh) {
        for (uint v: nbh) {
          if (u != v && !g->is_edge(u, v)) {
            return false;
          }
        }
      }
      return true;
    }

    bool check_solution(const std::vector<uint>& res) const {
      std::set<uint> candidates(res.begin(), res.end());
      bool symplical_found = false;
      while (!candidates.empty()) {
        symplical_found = false;
        for (uint v: candidates) {
          if (this->is_symplical(v, candidates)) {
            candidates.erase(v);
            symplical_found = true;
            break;
          }
        }
        if (!symplical_found) {
          return false;
        }
      }
      return true;
    }

    ChordalNew() {
      name = "ChordalNew";
      description = "Well, it's a chordal graph";
      shortcut = "-chn";
    }

    ChordalNew* clone() const {
        return new ChordalNew(*this);
    }
};

#endif
