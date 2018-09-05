#include <cstdio>
#include <string>
#include <algorithm>
#include <csignal>
#include <cstdlib>
#include "rds.h"
#include "graph.h"
#include "verifiers/verifiers.h"

using namespace std;

void show_usage(const char* argv)
{
  
  printf("Usage: %s [options] <dimacs input file>\nAvailable options:\n", argv);
  printf("\t-t\tTime limit in seconds (optional)\n");
  printf("\t-h|-?\tdisplay this help\n");
  printf("\t-weights <weights file>\n");
  printf("\t-comp\tuse graph complement\n");
  printf("Maximum Solvers, %ld available:\n", VerifierManager::getInstance()->verifiers.size());
  for(auto& element: VerifierManager::getInstance()->verifiers) {
    auto&& verifier = element.second();
    printf("\t%s\t%s\n",
           verifier->getShortcut().c_str(),
           verifier->getName().c_str());
    delete verifier;
  } 
  printf("Vertex ordering:\n");
  printf("\t-vd\tdegree from large to small\n");
  printf("\t-vd2\t2-neighborhood from large to small\n");
  printf("\t-vr\trandom ordering\n");
  printf("\t-vc n\tn-color ordering\n");
  printf("\t-vw\tweight from large to small\n");
  printf("\t-vrev\treverse ordering\n");
}

void pr_()
{
  printf("------------------------------------------------\n");
}

int main(int argc, char* argv[])
{
  printf("Russian-Doll-Search\nCopyright Eugene Lykhovyd, 2017.\n");
  pr_();
  if(argc < 2 || string(argv[1]) == "-h" || string(argv[1]) == "-?") {
    show_usage(argv[0]);
  } else {
    signal(SIGINT, print_lb_atomic); // from rds.h
    graph * g = from_dimacs(argv[argc-1]); // last argument must be filename
    if(!g)
    {
      fprintf(stderr, "Failure: no input graph, exiting...\n");
      return 1;
    }
    Verifier* v = 0;
    uint time_lim = 0;
    for(int i = 1; i < argc-1; ++i)
    {
      // so ugly, but switch refuses to compare strings
      if(string(argv[i]) == "-c") { if(v) delete v; v = new Clique(); }
      else if (string(argv[i]) == "-s") { if(v) delete v; v = new Stable(); }

      else if (string(argv[i]) == "-vd") { g->reorder_degree(); }
      else if (string(argv[i]) == "-vd2") { g->reorder_2nb(); }
      else if (string(argv[i]) == "-vr") { g->reorder_random(); }
      else if (string(argv[i]) == "-vw") { g->reorder_weight(); }
      else if (string(argv[i]) == "-vc") { g->reorder_color(atoi(argv[i+1])); i++; }
      else if (string(argv[i]) == "-vrev") { g->reorder_rev(); }

      else if (string(argv[i]) == "-t") { time_lim = atoi(argv[i+1]); i++; }
      else if (string(argv[i]) == "-comp") { graph* g_c = g->complement(); delete g; g = g_c; }
      else if (string(argv[i]) == "-weights") { g->read_weights(argv[i+1]); i++; }
      else {
          fprintf(stderr, "Wrong parameter: %s\n", argv[i]);
          if(v) delete v; delete g; return 1;
      }
    }
    if(!v)
    {
      fprintf(stderr, "No task specified\n");
      return 1;
    }
    vector<uint> res_p;
    uint res = rds(v, g, res_p, time_lim);
    printf("Solution verification: ");
    if(v->check_solution(g, res_p))
      printf("CORRECT\n");
    else
      printf("INCORRECT\n");
    g->restore_order(res_p);
    sort(res_p.begin(), res_p.end());
    pr_();
    printf("Solution:\n");
    for(uint i = 0; i < res_p.size(); ++i)
      printf("%u ", res_p[i]+1);
    printf("\nRDS returned res = %d\n", res);

/*    printf("Graph:\n");
    for(uint i = 0; i < res_p.size(); ++i)
      for(uint j = i+1; j < res_p.size(); ++j)
        if(g->is_edge(res_p[i], res_p[j]))
          printf("e %u %u\n", res_p[i]+1, res_p[j]+1); */
    delete g;
  }

  return 0;
}
