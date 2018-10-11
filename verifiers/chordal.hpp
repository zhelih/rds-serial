#ifndef _CHORDAL_H
#define _CHORDAL_H
#include "verifier.hpp"

class Chordal: public RegisterVerifier<Chordal> {
  std::vector<std::vector<unsigned int>> colors;
  std::vector<unsigned int> colors_to_update;

  unsigned int level = 0;

  public:
    bool check_pair(uint i, uint j) const {
      return true;
    }

    bool check(const std::vector<uint>& p, uint n) const {
//      printf("\nChecking if %d and (", n+1);
//      for(unsigned int i = 0; i < p.size(); ++i) {
//        printf("%d, ", p[i]+1);
//      }
//      printf("\b\b) form a chordal graph\n");
      std::vector<unsigned int> cnt_bottom(g->nr_nodes, 0);
      std::vector<bool> ok_left(g->nr_nodes, 0);

      for(auto& v: p) {
        for(auto& u: p) {
          if (v == u) {
            continue;
          }
//          printf("Checking pair %d, %d\n", v+1, u+1);
          if (!g->is_edge(n, v) || !g->is_edge(n, u)) {
//            printf("Verdict: no edge from %d to one of them\n", n+1);
            continue;
          }
          else {
//            printf("There are edges to both of them\n");
          }

          if (g->is_edge(v, u)) {
//            printf("Verdict: there is edge (%d, %d)\n", v+1, u+1);
            cnt_bottom[v]++;
//            printf("cnt_bottom[%d] now is %d\n", v+1, cnt_bottom[v]);
            continue;
          }

          if (colors[level][v] == colors[level][u]) {
//            printf("Verdict: (%d, %d, %d) is a claw. Solve it later\n", v+1, n+1, u+1);
            continue;
          }
        }
      }

      for(auto& v: p) {
        for(auto& u: p) {
          if (v == u) {
            continue;
          }
          if (g->is_edge(v, u)) {
//            printf("There is an edge (%d, %d), cnt_bottom[%d] = %d\n", v+1, u+1, v+1, cnt_bottom[v]);
            if (cnt_bottom[v] > 1) {
              ok_left[u] = true;
//              printf("ok_left[%d] = %d\n", u+1, (int)ok_left[u]);
            }
          }
        }
      }

      for(auto& v: p) {
        for(auto& u: p) {
          if (v == u) {
            continue;
          }
          if (g->is_edge(n, v) && g->is_edge(n, u) && !g->is_edge(v, u) && colors[level][v] == colors[level][u]) {
//            printf("Claw: (%d, %d, %d). ok_left[%d] = %d\n", v+1, n+1, u+1, v+1, (int)ok_left[v]);
            if (!ok_left[v]) {
//              printf("Verdict: no\n");
              return false;
            }
          }
        }
      }

//      printf("Verdict: yes\n");
      return true;
    }

    bool check_solution(const std::vector<uint>& res) const {
      return true;
    }

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

    Chordal() {
      name = "Chordal";
      description = "Well, it's a chordal graph";
      shortcut = "-ch";
    }

    Chordal* clone() const {
        return new Chordal(*this);
    }
};

#endif
