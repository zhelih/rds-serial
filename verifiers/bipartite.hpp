#ifndef _BIPARTITE_H
#define _BIPARTITE_H
#include "verifier.hpp"
#include <stack>

class Bipartite: public RegisterVerifier<Bipartite> {
  private:
    mutable std::vector<uint> color, s;

  public:
    inline bool check_pair(uint i, uint j) const {
      return true;
    }

    inline bool check(const std::vector<uint>& p, uint i, uint n) const {
      for(uint v: p)
        color[v] = 2;
      uint stack_size = 0;
      s[stack_size++] = n;
      color[n] = 1;
      while(stack_size > 0)
      {
        uint u = s[--stack_size];
        for(uint v: p)
        {
          if(g->is_edge(v, n) && color[v] == color[n])
            return false;
          if(g->is_edge(v, u)){
            if(color[v] == color[u])
              return false;
            if(color[v] == 2)
            {
              color[v] = !color[u];
              s[stack_size++] = v;
            }
          }
        }
      }
      return true;
    }

    bool check_solution(const std::vector<uint>& res) const {
      return true;
    }

    void init_aux(uint i, const std::vector<uint>& c) {
      s.resize(g->nr_nodes);
      color.resize(g->nr_nodes);
    }

    Bipartite() {
      name = "Bipartite";
      description = "Well, it's a bipartite graph";
      shortcut = "-b";
    }

    Bipartite* clone() const {
        return new Bipartite(*this);
    }
};

#endif
