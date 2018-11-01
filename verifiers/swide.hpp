#ifndef _SWIDE_H
#define _SWIDE_H
#include "verifier.hpp"
#include <algorithm>

class SWide: public RegisterVerifier<SWide> {
  private:
    uint s;
    std::vector<uint> is_end;

  public:
    bool check_pair(uint i, uint j) const {
      return true;
    }

    inline bool check(const std::vector<uint>& p, uint i, uint n) const {
      uint nr_ends = 0;
      for(uint i = 0; nr_ends < 3 && i < p.size(); ++i)
        if (g->is_edge(p[i], n)) {
          if (!is_end[p[i]])
            return false;
          else
            nr_ends++;
        }
      return (nr_ends < 3);
    }

    bool check_solution(const std::vector<uint>& res) const {
      return true;
    }

    void init_aux(uint i, const std::vector<uint>& c) {
      is_end.resize(g->nr_nodes, 0);
      is_end[i] = 1;
    }

    void prepare_aux(const std::vector<uint>& p, uint n, const std::vector<uint>& c)
    {
      uint nr_ends = 0;
      uint v_end[2];
      for(uint i = 0; nr_ends < 3 && i < p.size(); ++i) {
        if(g->is_edge(p[i], n) && is_end[p[i]])
            v_end[nr_ends++] = p[i];
        if(nr_ends == 2)
          break;
      }
    //  assert(nr_ends < 3);
      if(nr_ends == 1) // only change ends if 1
      {
        is_end[v_end[0]] = 0;
        is_end[n] = 1;
      }
      if(nr_ends == 2)
      {
        is_end[v_end[0]] = 0;
        is_end[v_end[1]] = 0;
      }
    }

    void undo_aux(const std::vector<uint>& p, uint n, const std::vector<uint>& c)
    {
      uint nr_ends = 0;
      is_end[n] = 0;
      for(uint v: p) {
        if(g->is_edge(v, n))
        {
          is_end[v] = 1;
          nr_ends++;
        }
        if(nr_ends == 2)
          break;
      }
    }

    SWide() {
      name = "s-Wide graph";
      description = "Well, it's an s-Wide graph";
      shortcut = "-sw";
      add_parameter("s", &s);
    }

    SWide* clone() const {
        return new SWide(*this);
    }
};

#endif
