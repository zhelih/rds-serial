#ifndef _FOREST_H
#define _FOREST_H
#include "verifier.hpp"
#include <stack>

class Forest: public RegisterVerifier<Forest> {
  private:
    mutable std::vector<uint> color, parent, s;

  public:
    bool check_pair(uint i, uint j) const {
      return true;
    }

    bool check(const std::vector<uint>& p, uint n) const {
      for(auto& v: p)
        color[v] = 0;
      uint stack_size = 0;
      s[stack_size++] = n;
      parent[n] = n;
      while(stack_size != 0)
      {
        uint v = s[--stack_size];
        color[v] = 1;
        if(n != parent[v] && g->is_edge(v, n))
          return false;
        for(auto& u: p) {
          if(u != parent[v] && g->is_edge(v, u))
          {
            if(color[u] == 1)
            {
              return false;
            } else {
              s[stack_size++] = u;
              parent[u] = v;
            }
          }
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
      color.resize(g->nr_nodes);
      parent.resize(g->nr_nodes);
      s.resize(g->nr_nodes);
    }

    Forest() {
      name = "Forest";
      description = "Well, it's a forest";
      shortcut = "-f";
    }

    Forest* clone() const {
        return new Forest(*this);
    }
};

#endif
