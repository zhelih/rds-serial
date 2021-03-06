#include <string>
#include <algorithm>
#include <functional>
#include <csignal>
#include <iostream>
#include "rds/rds.hpp"
#include "io/output.hpp"
#include "io/parameters.hpp"
#include <fstream>

extern const char* gitversion;

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
  fprintf(stderr, "Russian-Doll-Search, build %s\n\n", gitversion);
  pr_();

  if(argc < 2 || std::string(argv[1]) == parameters::PARAM_HELP || std::string(argv[1]) == "-?") {
    parameters::show_usage(argv[0]);
    return 0;
  }
  signal(SIGINT, print_lb_atomic); 
  std::string filename(argv[argc-1]);
  auto processor = parameters::parse_args(argc, argv);
  std::cerr<<"Parsed processor parameters"<<std::endl;
  auto graphs = parameters::parse_is_batch(argc, argv)?
    (get_graphs_names(filename)):(std::vector<std::string>{filename});
  using namespace std::placeholders;
  auto out = std::bind(parameters::parse_to_latex(argc, argv)?(output::latex):(output::fancy), ref(std::cout), _1);
  main_batch(graphs, processor, out);

  return 0;
}
