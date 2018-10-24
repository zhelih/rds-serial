#include <string>
#include <algorithm>
#include <functional>
#include <csignal>
#include <iostream>
#include "graph.h"
#include "verifiers/verifiers.h"
#include "rds.h"
#include "utils.hpp"
#include "io/output.hpp"
#include "io/parameters.hpp"
#include <fstream>
using namespace std::placeholders;

algorithm_run run_rds(std::shared_ptr<verifier> v, ordering order, bool reverse, bool complement, unsigned int time_limit, const std::string& graph_file) {
  algorithm_run result;
  result.graphname = graph_file;
  result.reverse = reverse;
  result.time_limit = time_limit;
  result.complement = complement;

  std::cerr<<"Reading graph"<<std::endl;
  graph *g = from_dimacs(graph_file.c_str());
  if (!g) {
    result.valid = false;
    return result;
  }

  if (complement) {
    graph *ng = g->complement();
    delete g;
    g = ng;
  }

  g->apply_order(order, reverse);
  std::cerr<<"Done"<<std::endl;
  v->bind_graph(g);
  std::cerr<<"Calling rds...";
  rds(v.get(), g, result);
  std::cerr<<"Done"<<std::endl;

  result.correct = v->check_solution(result.certificate);
  g->restore_order(result.certificate);
  std::sort(result.certificate.begin(), result.certificate.end());
  delete g;
  return result;
}

std::vector<std::string> get_graphs_names(const std::string& batchname) {
  std::ifstream batch(batchname);
  std::vector<std::string> result;
  std::copy(line_iter{batch}, line_iter{}, std::back_inserter(result));
  return result;
}

void main_batch(const std::vector<std::string>& graphs, std::function<algorithm_run(std::string)> solve, std::function<void(const algorithm_run&)> save) {
  std::vector<algorithm_run> results;
  std::transform(graphs.begin(), graphs.end(), std::back_inserter(results), solve);
  std::for_each(results.begin(), results.end(), save);
}

int main(int argc, char* argv[])
{
  fprintf(stderr, "Russian-Doll-Search\nCopyright Eugene Lykhovyd, 2017.\n");
  pr_();

  if(argc < 2 || std::string(argv[1]) == parameters::PARAM_HELP || std::string(argv[1]) == "-?") {
    parameters::show_usage(argv[0]);
    return 0;
  }
  signal(SIGINT, print_lb_atomic); // from rds.h  
  std::string filename(argv[argc-1]);
  auto processor = parameters::parse_args(run_rds, argc, argv);
  std::cerr<<"Parsed processor parameters"<<std::endl;
  auto graphs = parameters::parse_is_batch(argc, argv)?
    (get_graphs_names(filename)):(std::vector<std::string>{filename});
  auto out = std::bind(parameters::parse_to_latex(argc, argv)?(output::latex):(output::fancy), ref(std::cout), _1);
  main_batch(graphs, processor, out);

  return 0;
}
