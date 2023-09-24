#ifndef _DISTRICTING_H
#define _DISTRICTING_H
#include "verifier.hpp"

#include <memory> // for std::shared_ptr
#include <cstdio>
#include <vector>
#include <cmath>
#include <stack>
#include <set>
#include "../rank.hpp"

void calculate_UL(const std::vector<int>& population, int k, int* L, int* U)
{
  int total_pop = 0;
  for(int p : population)
    total_pop += p;
  if(*U == 0)
     *U = static_cast<int>(floor(static_cast<float>(total_pop) / static_cast<float>(k) * 1.005));
  if(*L == 0)
    *L = static_cast<int>(ceil(static_cast<float>(total_pop) / static_cast<float>(k) * 0.995));
}

class Districting: public RegisterVerifier<Districting> {
  public:
    inline bool check_pair(uint i, uint j) const {
      if (population->at(i) >= L || population->at(j) >= L)
          return false;
      if (g->is_edge(i,j))
          return (population->at(i) + population->at(j)) < L;
      return true;
    }

    inline bool check(const std::vector<uint>& p, uint i, uint n) const {
      // set P \u i guaranteed
      // check if n can be added
      int weight = population->at(n);
      std::set<uint> willJoin;
      for(uint pp : p)
      {
        if(g->is_edge(pp, n)) {
          willJoin.insert(Find(aux_sets[level], pp));
        }
      }
      for (auto pp: willJoin) {
        weight += set_weight[level][pp];
      }
      return weight < L;
    }

    ///////////////////// AUX INFO
    int level;
    std::vector<union_of_sets> aux_sets; // per level
    std::vector<std::vector<int>> set_weight; // per level
    inline void init_aux(uint _, const std::vector<uint>& c) {
        // init all data and level 0
        // create union of sets structure
        int nr_nodes = g->nr_nodes;
        aux_sets.resize(nr_nodes);
        for(int i = 0; i < nr_nodes; ++i)
          aux_sets[i] = InitUnionOfSets(nr_nodes);
        for(int j = 0; j < nr_nodes; ++j)
          MakeSet(aux_sets[0], j); // init as singletone nodes

        set_weight.resize(nr_nodes); // per level
        for(int i = 0; i < nr_nodes; ++i)
          set_weight[i].resize(nr_nodes); // each element is weight of Find(i)
        for(int i = 0; i < nr_nodes; ++i)
          set_weight[0][i] = population->at(i);
        level = 0;
    };
    inline void prepare_aux(const std::vector<uint>& p, uint j, const std::vector<uint>& c, uint c_start) {
        // cur is level
        // next is level+1
        // prepare next

        // vertex j is being added to p
        // merge all sets neghbors of j
        // we ignore set C here
        FreeUnionOfSets(aux_sets[level+1]);
        //copy
        aux_sets[level+1] = InitUnionOfSets(g->nr_nodes);
        set_weight[level+1] = set_weight[level];
        for(int i = 0; i < g->nr_nodes; ++i)
        {
            aux_sets[level+1].rank[i] = aux_sets[level].rank[i];
            aux_sets[level+1].dad[i] = aux_sets[level].dad[i];
        }

        int newWeight = population->at(j);
        for (auto pp: p) {
          if (!g->is_edge(pp, j)) continue;
          int set_j = Find(aux_sets[level+1], j);
          int set_pp = Find(aux_sets[level+1], pp);
          if (set_j != set_pp) {
            newWeight += set_weight[level][set_pp];
            Union(aux_sets[level+1], set_j, set_pp);
          }
        }
        set_weight[level+1][Find(aux_sets[level+1], j)] = newWeight;

        level++;
    };
    inline void undo_aux(const std::vector<uint>& p, uint j, const std::vector<uint>& c, uint c_start) {
        FreeUnionOfSets(aux_sets[level]);
        level--;
    };
    inline void free_aux() { level = 0; };
    ///////////////////////

    inline bool check_solution(const std::vector<uint>& res) const {
				int nr_nodes = g->nr_nodes;
			  // run DFS to find connected components
		    std::vector<int> comp(nr_nodes, -1); // [i] component
    		int nr_comp = 0; // number of connected components
		    std::stack<int> s; // stack for the DFS
    		std::vector<int> visited(nr_nodes, 0);
		    for (int i : res) // start DFS
    		{
		        if (!visited[i])
    		    {
        		    s.push(i);
            		while (!s.empty())
		            {
    		            int v = s.top(); s.pop();
        		        if (!visited[v])
            		    {
                		    visited[v] = true;
		                    comp[v] = nr_comp;
                        for (auto&& it = g->nbh_begin(v); it != g->nbh_end(v); ++it)
                        {
    		                //for (int nb_v : nb(v))
                            int nb_v = *it;
                            if (std::find(res.begin(), res.end(), nb_v) != res.end()) {
                		            if (!visited[nb_v])
                    		            s.push(nb_v);
                            }
		                    }
    		            }
        		    }
            		nr_comp++;
		        }
    		}
				std::vector<int> comp_weight(nr_comp, 0);
        for (int i = 0; i < nr_nodes; ++i)
            if (comp[i] != -1)
               comp_weight[comp[i]] += population->at(i);
        for (int i = 0; i < nr_nodes; ++i) {
          if (comp[i] == -1) continue;
          printf("Node %d belongs to connected component %d and has weight %d\n", i, comp[i], population->at(i));
        }
        for (int i = 0; i < nr_comp; ++i)
            if (comp_weight[i] >= L)
                return false;
 	      return true;
    }

    Districting() {
      name = "Districting";
      description = "Districting requested by Austin Buchanan";
      shortcut = "-distr";

      int g_nr_nodes = 105;//NB TODO FIXME WOMBAT
      // for some reason g can be nullptr if uninitialized
      if (g_nr_nodes > 0)
      {
          // TODO read from input the state
          const char* state = "KS"; //FIXME
          int k = 4; //FIXME
          char buf[1023];
          snprintf(buf, sizeof(buf), "districting/%s/counties/graph/%s.population", state, state);
          FILE *f = fopen(buf, "r");
          if (!f) {
              printf("Failed to open data for %s\n", state);
              throw buf;
          }

          std::cerr << "done" << std::endl;

          population.reset(new std::vector<int>(g_nr_nodes));
          fgets(buf, sizeof(buf), f); // skip 1st line
          for(int i = 0; i < g_nr_nodes; ++i) {
              int node, pop;
              fscanf(f, "%d %d ", &node, &pop);
              population->at(node) = pop;
          }
          fclose(f);

          int U; //dummy
          calculate_UL(*population, k, &L, &U);
          printf("Districting verified init completed, L = %d\n", L);
      }
    }

    // share data across instances
    std::shared_ptr<std::vector<int>> population;
    int L;

    Districting* clone() const {
        return new Districting(*this);
    }
};

#endif
