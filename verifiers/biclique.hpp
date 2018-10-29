#ifndef _BICLIQUE_H
#define _BICLIQUE_H
#include "verifier.hpp"

class Biclique: public RegisterVerifier<Biclique> {
  public:
    inline bool check_pair(uint i, uint j) const {
      return true;
    }

    inline bool check(const std::vector<uint>& p, uint i, uint n) const {
      if(p.size() <= 1)
        return true;
      return !(g->is_edge(p.front(), n) ^ g->is_edge(i, p.front()) ^ g->is_edge(n,i));
    }

    bool check_solution(const std::vector<uint>& res) const {
      std::vector<uint> left;
      std::vector<uint> right;
      left.push_back(res.front());
      for(uint v: res) {
        if(v == res.front())
          continue;
        if(g->is_edge(res.front(), v))
          right.push_back(v);
        else
          left.push_back(v);
      }
      // check all in left are disconnected
      for(uint v1 : left)
        for(uint v2 : left)
          if(v1 == v2)
            continue;
          else if(g->is_edge(v1,v2))
            return false;
      // check all in right are disconnected
      for(uint v1 : right)
        for(uint v2 : right)
          if(v1 == v2)
            continue;
          else if(g->is_edge(v1,v2))
            return false;
      // check all between right and left are connected
      for(uint v1 : left)
        for(uint v2 : right)
          if(!g->is_edge(v1,v2))
            return false;
      return true;
    }

    Biclique() {
      name = "Biclique";
      description = "Well, it's a Biclique";
      shortcut = "-bc";
    }

    Biclique* clone() const {
        return new Biclique(*this);
    }
};

#endif
