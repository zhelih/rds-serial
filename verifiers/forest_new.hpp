#ifndef _FOREST_NEW_H
#define _FOREST_NEW_H
#include "verifier.hpp"
#include <stack>

class ForestNew: public RegisterVerifier<ForestNew> {
  private:
    mutable std::vector<bool> connected;
    mutable std::vector<std::vector<unsigned int>> p;
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
      for(auto& v: P) {
        if(g->is_edge(v, n)) {
          unsigned int actualV = get_p(currentP, v);
          if (connected[actualV]) return false;
          connected[actualV] = true;
        }
      }
      return true;
    }

    bool check_solution(const std::vector<uint>& res) const {
      std::stack<std::pair<uint,uint> > s;
      std::vector<uint> color(g->nr_nodes);
      for(uint i = 0; i < g->nr_nodes; ++i)
        color[i] = 0;
      for(auto& v: res)
        if(color[v] == 0) // undiscovered
        {
          // start BFS from j
          s.push(std::make_pair(v, v));
          while(!s.empty())
          {
            std::pair<uint,uint> pair = s.top(); s.pop();
            color[pair.first] = 1;
            for(auto& u: res)
              if(u != pair.second && g->is_edge(pair.first, u)) {
                if(color[u] == 1)
                  return false;
                else
                  s.push(std::make_pair(u,pair.first));
              }
          }
        }
      return true;
    }

    void init_aux(uint i, const std::vector<uint>& c) {
      p.resize(g->nr_nodes);
      connected.resize(g->nr_nodes);
      for(auto& v: p) v.resize(g->nr_nodes);
      for(unsigned int i = 0; i < p[0].size(); ++i) p[0][i] = i;

      level = 0;
    }

    void prepare_aux(const std::vector<uint>& P, uint j, const std::vector<uint>& c)
    {
      auto& currentP = p[level];
      auto& nextP = p[level+1];
      nextP = currentP;
      int newC = -1;
      for(auto& v: P) {
        if (g->is_edge(v, j)) {
          if (newC >= 0) {
            join_p(nextP, newC, v);
          }
          else {
            newC = v;
          }
        }
      }
      
      if (newC >= 0) {
        join_p(nextP, newC, j);
      }

      level++;
    }

    void undo_aux(const std::vector<uint>& p, uint j, const std::vector<uint>& c)
    {
      level--;
    }

    ForestNew() {
      name = "Forest new";
      description = "Well, it's a forest";
      shortcut = "-fn";
    }

    ForestNew* clone() const {
        return new ForestNew(*this);
    }
};

#endif
