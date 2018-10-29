#ifndef _CHORDALNEW_H
#define _CHORDALNEW_H
#include "verifier.hpp"
#include <deque>
#include <algorithm>
#include <iterator>
#include <set>

class ChordalNew: public RegisterVerifier<ChordalNew> {
  std::vector<std::vector<unsigned int>> colors;
  std::vector<unsigned int> colors_to_update;

  unsigned int level = 0;

  public:
    bool check_pair(uint i, uint j) const {
      return true;
    }

    inline bool check(const std::vector<uint>& p, uint i, uint n) const {
      std::deque<unsigned int> nbh;
      std::vector<bool> reachable(g->nr_nodes, false);
      std::copy_if(p.begin(), p.end(), std::back_inserter(nbh), [&](uint v){return g->is_edge(n, v);});
      while (!nbh.empty()) {
        auto v = nbh.front();
        nbh.pop_front();
        for (auto& u: p) {
          if (!g->is_edge(n, u) && g->is_edge(v, u)) {
            if (reachable[u]) return false;
            reachable[u] = true;
            nbh.push_back(u);
          }
        }
      }
      return true;
    }

    bool is_symplical(const unsigned int& v, const std::set<unsigned int>& vertices) const {
      std::vector<unsigned int> nbh;
      std::copy_if(vertices.begin(), vertices.end(), std::back_inserter(nbh), [&](uint u){return g->is_edge(v, u);});
      std::cout<<"Checking if vertex "<<v+1<<" is symplical"<<std::endl;
      std::cout<<"v's nbh is ";
      std::copy(nbh.begin(), nbh.end(), std::ostream_iterator<uint>(std::cout, "+1,"));
      std::cout<<std::endl;
      for (auto& u: nbh) {
        for (auto& v: nbh) {
          if (u != v && !g->is_edge(u, v)) {
            std::cout<<"There is no edge from "<<u+1<<" to "<<v+1<<std::endl;
            return false;
          }
        }
      }
      return true;
    }

    bool check_solution(const std::vector<uint>& res) const {
      std::set<unsigned int> candidates(res.begin(), res.end());
      bool symplical_found = false;
      while (!candidates.empty()) {
        symplical_found = false;
        for (auto& v: candidates) {
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
/*
    void init_aux(uint i, const std::vector<uint>& c) {
      colors.resize(g->nr_nodes); 
      for(auto& v: colors) {
        v.resize(g->nr_nodes, 0);
      }
      for(unsigned int i = 0; i < g->nr_nodes; ++i) {
        colors[0][i] = i;
      }
      
      colors_to_update.reserve(g->nr_nodes);
    }

    void prepare_aux(const std::vector<uint>& p, uint j, const std::vector<uint>& c)
    {
      ++level;
      colors[level] = colors[level-1];
      colors[level][j] = j;
      for(auto& v: p) {
        if (g->is_edge(v, j)) {
          colors_to_update.push_back(colors[level][v]);
        }
      }
      for(auto& c: colors_to_update) {
        for(auto& ccolor: colors[level]) {
          if (ccolor == c) {
            ccolor = j;
          }
        }
      }
      colors_to_update.resize(0);
    }

    void undo_aux(const std::vector<uint>& p, uint j, const std::vector<uint>& c)
    {
      --level;
    }

    void free_aux() {
      level = 0;
    }
*/
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
