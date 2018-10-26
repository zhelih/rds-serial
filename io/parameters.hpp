#ifndef _PARAMETERS_HPP
#define _PARAMETERS_HPP
#include <vector>
#include "../verifiers/verifiers.h"
#include <cstdio>
#include <memory>

namespace parameters {
static const std::string PARAM_REVERSE = "-vrev";
static const std::string PARAM_TIMELIM = "-t";
static const std::string PARAM_BATCH   = "-B";
static const std::string PARAM_HELP    = "-h";
static const std::string PARAM_LATEX   = "-L";
static const std::string PARAM_COMPL   = "-comp";

void show_usage(const char* argv)
{
  printf("Usage: %s [options] <dimacs input file>\n", argv);
  printf("Usage: %s [options] -B <batch file>\nAvailable options:\n", argv);
  std::cout<<"\t"<<PARAM_TIMELIM<<"\tTime limit in seconds (optional)"<<std::endl;
  std::cout<<"\t"<<PARAM_HELP<<"\tDisplay this help"<<std::endl;
//  printf("\t-weights <weights file>\n");
  std::cout<<"\t"<<PARAM_COMPL<<"\tUse graph complement"<<std::endl;
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
  printf("\t-vclq\torder from Cliquer\n");
  printf("\t-vrev\treverse ordering\n");
  printf("Batches:\n");
  std::cout<<"\t"<<PARAM_BATCH<<"\tread a list of dimacs input files (one file per line)."<<std::endl;
  printf("Output:\n");
  std::cout<<"\t"<<PARAM_LATEX<<"\tproduce report in a form of LaTeX table"<<std::endl;
}

RDSMethod parse_verifier(const int argc, const char* const argv[]) {
  for (int i = 1; i < argc - 1; ++i) {
    std::string arg(argv[i]);
    if (VerifierManager::instance()->is_shortcut(arg))
      return VerifierManager::instance()->get_rds(arg);
  }
  throw std::invalid_argument("No task specified");
}

ordering parse_order(const int argc, const char* const argv[]) {
  for (int i = 1; i < argc-1; ++i) {
    std::string arg(argv[i]);
    if (arg == std::string("-vd")) return ordering::Degree;
    if (arg == std::string("-vd2")) return ordering::Neighborhood;
    if (arg == std::string("-vdg")) return ordering::Degeneracy;
    if (arg == std::string("-vc")) return ordering::NColor;
    if (arg == std::string("-vclq")) return ordering::Cliquer;
  }
  return ordering::None;
}

bool parse_reverse(const int argc, const char* const argv[]) {
  for (int i = 1; i < argc-1; ++i) {
    std::string arg(argv[i]);
    if (arg == PARAM_REVERSE) return true;
  }
  return false;
}

unsigned int parse_time_limit(const int argc, const char* const argv[]) {
  for (int i = 1; i < argc-1; ++i) {
    std::string arg(argv[i]);
    if (arg == PARAM_TIMELIM) {
      return std::stoi(argv[i+1]);
    }
  }
  return 0;
}

bool parse_is_batch(const int argc, const char* const argv[]) {
  for (int i = 1; i < argc-1; ++i) {
    std::string arg(argv[i]);
    if (arg == PARAM_BATCH) {
      return true;
    }
  }
  return false;
}

bool parse_to_latex(const int argc, const char* const argv[]) {
  for (int i = 1; i < argc-1; ++i) {
    std::string arg(argv[i]);
    if (arg == PARAM_LATEX) {
      return true;
    }
  }
  return false;
}

bool parse_complement(const int argc, const char* const argv[]) {
  for (int i = 1; i < argc-1; ++i) {
    std::string arg(argv[i]);
    if (arg == PARAM_COMPL) {
      return true;
    }
  }
  return false;
}

std::function<algorithm_run(std::string)> parse_args(const int argc, const char* const argv[]
) {
  using namespace std::placeholders;
  try {
    return std::bind(parse_verifier(argc, argv),
                     parse_order(argc, argv), parse_reverse(argc, argv), parse_complement(argc, argv),
                     parse_time_limit(argc, argv), _1
                    );
  }
  catch (const std::invalid_argument& error) {
    std::cerr<<error.what()<<std::endl;
    exit(1);
  }
}

}

#endif
