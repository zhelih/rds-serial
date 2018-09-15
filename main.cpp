#include <cstdio>
#include <string>
#include <algorithm>
#include <csignal>
#include <cstdlib>
#include "rds.h"
#include "graph.h"
#include "verifiers/verifiers.h"

void show_usage(const char* argv)
{
  
  printf("Usage: %s [options] <dimacs input file>\nAvailable options:\n", argv);
  printf("\t-t\tTime limit in seconds (optional)\n");
  printf("\t-h|-?\tdisplay this help\n");
  printf("\t-weights <weights file>\n");
  printf("\t-comp\tuse graph complement\n");
  printf("Maximum Solvers, %ld available:\n", VerifierManager::instance()->count());
  for(auto& element: VerifierManager::instance()->verifiers) {
    auto&& verifier = element.second();
    printf("\t%s", verifier->get_shortcut().c_str());
    for(unsigned int i = 0; i < verifier->number_of_parametes(); ++i) {
      printf(" <%s>", verifier->get_parameter_name(i).c_str());
    }
    printf("\t%s\n", verifier->get_name().c_str());
  } 
  printf("Vertex ordering:\n");
  printf("\t-vd\tdegree from large to small\n");
  printf("\t-vdg\tdegeneracy order\n");
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

bool parse_for_verifier_parameters(verifier*& v, const int& argc, char* argv[], int& position) {
  if (position >= argc-1) {
    return false;
  }
  auto&& arg = std::string(argv[position]);
  if (VerifierManager::instance()->is_shortcut(arg)) {
    delete v;
    v = VerifierManager::instance()->create(arg);
    for(uint p = 0; p < v->number_of_parametes(); ++p) {
      int parameter;
      try {
        parameter = std::stoi(argv[++position]);
      } catch (const std::invalid_argument&) {
        fprintf(stderr, "Verifier: Wrong parameter: %s\n", argv[position-p-1]);
        exit(1);
      }
      v->provide_parameter(parameter);
    }
    return true;
  }
  return false;
}

bool parse_for_vertex_order_parameters(graph* g, const int& argc, char* argv[], int& position) {
  if (position >= argc-1) {
    return false;
  }
  std::string arg = std::string(argv[position]);
  if (arg == "-vd") { g->reorder_degree(); return true;}
  else if (arg == "-vd2") { g->reorder_2nb(); return true;}
  else if (arg == "-vdg") { g->reorder_degeneracy(); return true;}
  else if (arg == "-vr") { g->reorder_random(); return true;}
  else if (arg == "-vw") { g->reorder_weight(); return true;}
  else if (arg == "-vc") { g->reorder_color(atoi(argv[++position])); return true;}
  else if (arg == "-vrev") { g->reorder_rev(); return true;}
  else {
    return false;
  }
}

void verify_solution(verifier*& v, std::vector<uint>& sol) {
  printf("Solution verification: ");
  if(v->check_solution(sol))
    printf("CORRECT\n");
  else
    printf("INCORRECT\n");
}

int main(int argc, char* argv[])
{
  printf("Russian-Doll-Search\nCopyright Eugene Lykhovyd, 2017.\n");
  pr_();

  if(argc < 2 || std::string(argv[1]) == "-h" || std::string(argv[1]) == "-?") {
    show_usage(argv[0]);
    return 0;
  }

  signal(SIGINT, print_lb_atomic); // from rds.h
  graph * g = from_dimacs(argv[argc-1]); // last argument must be filename
  if(!g)
  {
    fprintf(stderr, "Failure: no input graph, exiting...\n");
    return 1;
  }

  verifier* v = nullptr;
  uint time_lim = 0;
  for(int i = 1; i < argc-1; ++i)
  {
    // so ugly, but switch refuses to compare strings
    if (parse_for_verifier_parameters(v, argc, argv, i)) continue;
    if (parse_for_vertex_order_parameters(g, argc, argv, i)) continue;
    if (i >= argc-1) {
      break;
    }
    auto&& arg = std::string(argv[i]);
    if (arg == "-t") { time_lim = atoi(argv[i+1]); i++; }
    else if (arg == "-comp") { graph* g_c = g->complement(); delete g; g = g_c; }
    else if (arg == "-weights") { g->read_weights(argv[i+1]); i++; }
    else {
        fprintf(stderr, "Wrong parameter: %s\n", argv[i]);
        delete v;
        delete g;
        return 1;
    }
  }

  if(!v)
  {
    fprintf(stderr, "No task specified\n");
    return 1;
  }

  v->bind_graph(g);

  std::vector<uint> res_p;
  uint res = rds(v, g, res_p, time_lim);

  verify_solution(v, res_p);

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

  return 0;
}
