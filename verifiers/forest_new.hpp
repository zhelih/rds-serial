#ifndef _FOREST_NEW_H
#define _FOREST_NEW_H
#include "verifier.hpp"
#include <stack>

class ForestNew: public RegisterVerifier<ForestNew> {
  private:
    mutable std::vector<std::vector<uint>> p_;
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

    bool check_pair(uint i, uint j) const {
      return true;
    }

    inline bool check(const std::vector<uint>& P, uint i, uint n) const {
      auto& currentP = p_[level];
      uint ip = get_p(currentP, i);
      bool encountered = false;
      for (uint v: P) {
        if (g->is_edge(v, n) && get_p(currentP, v) == ip) {
          if (encountered) return false;
          encountered = true;
        }
      }
      return true;
    }

    bool check_solution(const std::vector<uint>& res) const {
      std::stack<std::pair<uint,uint> > s;
      std::vector<uint> color(g->nr_nodes);
      for(uint i = 0; i < g->nr_nodes; ++i)
        color[i] = 0;
      for(uint v: res)
        if(color[v] == 0) // undiscovered
        {
          // start BFS from j
          s.push(std::make_pair(v, v));
          while(!s.empty())
          {
            std::pair<uint,uint> pair = s.top(); s.pop();
            color[pair.first] = 1;
            for(uint u: res)
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
      p_.resize(g->nr_nodes);
      for(auto& v: p_) v.resize(g->nr_nodes);
      for(uint i = 0; i < p_[0].size(); ++i)
        p_[0][i] = i;

      level = 0;
    }

    void prepare_aux(const std::vector<uint>& P, uint j, const std::vector<uint>& c)
    {
      auto& currentP = p_[level];
      auto& nextP = p_[level+1];
      nextP = currentP;
      int newC = -1;
      for(uint v: P) {
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
