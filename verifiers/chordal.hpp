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
      std::deque<uint> nbh;
      std::vector<bool> reachable(g->nr_nodes, false);
      std::copy_if(p.begin(), p.end(), std::back_inserter(nbh), [&](uint v){return g->is_edge(n, v);});
      while (!nbh.empty()) {
        uint v = nbh.front();
        nbh.pop_front();
        for (uint u: p) {
          if (!g->is_edge(n, u) && g->is_edge(v, u)) {
            if (reachable[u]) return false;
            reachable[u] = true;
            nbh.push_back(u);
          }
        }
      }
      return true;
    }

    bool is_symplical(const uint v, const std::set<unsigned int>& vertices) const {
      std::vector<uint> nbh;
      std::copy_if(vertices.begin(), vertices.end(), std::back_inserter(nbh), [&](uint u){return g->is_edge(v, u);});
      std::cout<<"Checking if vertex "<<v+1<<" is symplical"<<std::endl;
      std::cout<<"v's nbh is ";
      std::copy(nbh.begin(), nbh.end(), std::ostream_iterator<uint>(std::cout, "+1,"));
      std::cout<<std::endl;
      for (uint u: nbh) {
        for (uint v: nbh) {
          if (u != v && !g->is_edge(u, v)) {
            std::cout<<"There is no edge from "<<u+1<<" to "<<v+1<<std::endl;
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
