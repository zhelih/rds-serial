#ifndef _BIPARTITE_NEW_H
#define _BIPARTITE_NEW_H
#include "verifier.hpp"
#include <stack>
#include <algorithm>
#include <cstdlib>
#include <cstdio>
#include <cstring>

class BipartiteNew: public RegisterVerifier<BipartiteNew> {
  private:
    mutable std::vector<bool> connected;
    mutable std::vector<std::vector<uint>> p_;
    std::vector<std::vector<int>> second_part;
    uint level;

  public:
    uint get_p(std::vector<uint>& p, const uint v) const {
      return (v == p[v])?v:(p[v] = get_p(p, p[v]));
    }

    void join_p(std::vector<uint>& p, uint a, uint b) {
      a = get_p(p, a);
      b = get_p(p, b);
      if (a != b) {
        p[a] = b;
      }
    }

    inline bool check_pair(uint i, uint j) const {
      return true;
    }

    inline bool check(const std::vector<uint>& P, uint i, uint n) const {
      auto& currentP = p_[level];
      auto& currentSP = second_part[level];

      int iP = get_p(currentP, i), sP = currentSP[iP];
      if (sP == -1) return true;
      
      sP = get_p(currentP, currentSP[iP]);
      bool encounteredLeft = false, encounteredRight = false;
      for (uint v: P) {
        if (g->is_edge(v, n)) {
          int vP = get_p(currentP, v);
          if (vP == iP) encounteredLeft = true;
          if (vP == sP) encounteredRight = true;
          if (encounteredLeft && encounteredRight) return false;
        }
      }
      return true;
    }

    bool check_solution(const std::vector<uint>& res) const {
      if (res.empty()) return true;
      std::vector<uint> color(g->nr_nodes, 0);
      std::deque<uint> dfs;
      bool keepgoing = false;
      do {
        keepgoing = false;
        for (uint v: res) {
          if (!color[v]) {
            dfs.push_front(v);
            keepgoing = true;
	    break;
	  }
        }
        while (!dfs.empty()) {
          uint v = dfs.front();
          dfs.pop_front();
          if (!color[v]) color[v] = 1;
          for (uint u: res) {
            if (g->is_edge(u, v)) {
              if (color[u]) {
                if (color[v] == color[u]) {
                  return false;
                }
              }
              else {
                color[u] = 3 - color[v];
                dfs.push_front(u);
              }
            }
          }
        }
      } while (keepgoing);
      return true;
    }

    void init_aux(uint i, const std::vector<uint>& c) {
      p_.resize(g->nr_nodes);
      connected.resize(g->nr_nodes);
      for(auto& v: p_) v.resize(g->nr_nodes);
      for(uint i = 0; i < p_[0].size(); ++i) p_[0][i] = i;

      second_part.resize(g->nr_nodes);
      for(auto& v: second_part) v.resize(g->nr_nodes);
      for(uint i = 0; i < second_part[0].size(); ++i) second_part[0][i] = -1;

      level = 0;
    }

    void prepare_aux(const std::vector<uint>& P, uint j, const std::vector<uint>& c, uint c_start)
    {
      auto& currentP = p_[level];
      auto& currentSP = second_part[level];
      auto& nextP = p_[level+1];
      auto& nextSP = second_part[level+1];
      nextP = currentP;
      nextSP = currentSP;
      int newC = -1;
      for(uint v: P) {
        if (g->is_edge(v, j)) {
          if (currentSP[v] == -1) {
            nextSP[v] = j;
          }

          if (newC >= 0) {
            join_p(nextP, newC, v);
            join_p(nextP, nextSP[newC], nextSP[v]);
          }
          else {
            newC = v;
          }
        }
      }

      if (newC >= 0) {
        join_p(nextP, nextSP[newC], j);
        nextSP[j] = newC;
      }

      level++;
    }

    void undo_aux(const std::vector<uint>& p, uint j, const std::vector<uint>& c, uint c_start)
    {
      level--;
    }

    inline void free_aux() const {}

    BipartiteNew() {
      name = "Bipartite new";
      description = "Well, it's a bipartite graph";
      shortcut = "-bn";
    }

    BipartiteNew* clone() const {
        return new BipartiteNew(*this);
    }
};

#endif
