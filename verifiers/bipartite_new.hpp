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
    mutable std::vector<std::vector<unsigned int>> p;
    std::vector<std::vector<int>> second_part;
    unsigned int level;

  public:
    unsigned int get_p(std::vector<uint>& p, const unsigned int v) const {
      return (v == p[v])?v:(p[v] = get_p(p, p[v]));
    }

    void join_p(std::vector<unsigned int>& p, unsigned int a, unsigned int b) {
      a = get_p(p, a);
      b = get_p(p, b);
      if (a != b) {
        p[a] = b;
      }
    }

    bool check_pair(uint i, uint j) const {
      return true;
    }

    bool check(const std::vector<uint>& P, uint n) const {
      connected.assign(g->nr_nodes, false);
      auto& currentP = p[level];
      auto& currentSP = second_part[level];
      for(auto& v: P) {
        if(g->is_edge(v, n)) {
          unsigned int actualV = get_p(currentP, v);
          connected[actualV] = true;

          if(currentSP[actualV] >= 0 && connected[get_p(currentP, currentSP[actualV])]) {
            return false;
          }
        }
      }
      return true;
    }

    bool check_solution(const std::vector<uint>& res) const {
      return true;
    }

    void init_aux(uint i, const std::vector<uint>& c) {
      p.resize(g->nr_nodes);
      connected.resize(g->nr_nodes);
      for(auto& v: p) v.resize(g->nr_nodes);
      for(unsigned int i = 0; i < p[0].size(); ++i) p[0][i] = i;

      second_part.resize(g->nr_nodes);
      for(auto& v: second_part) v.resize(g->nr_nodes);
      for(unsigned int i = 0; i < second_part[0].size(); ++i) second_part[0][i] = -1;

      level = 0;
    }

    void prepare_aux(const std::vector<uint>& P, uint j, const std::vector<uint>& c)
    {
      auto& currentP = p[level];
      auto& currentSP = second_part[level];
      auto& nextP = p[level+1];
      auto& nextSP = second_part[level+1];
      nextP = currentP;
      nextSP = currentSP;
      int newC = -1;
      for(auto& v: P) {
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

    void undo_aux(const std::vector<uint>& p, uint j, const std::vector<uint>& c)
    {
      level--;
    }

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
