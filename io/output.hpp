#ifndef _OUTPUT_HPP
#define _OUTPUT_HPP
#include <iostream>
#include "../utils.hpp"
#include "../rds/rds_utils.hpp"

namespace output
{
void fancy(std::ostream& out, const algorithm_run& run) {
  pr_();
  out<<"RDS run for "<<run.graphname;
  if (run.complement) {
    out<<"'s complement";
  }
  if (!run.valid) {
    out<<" failed."<<std::endl;
    return;
  }
  else {
    out<<" was"<<(run.complete?"":" NOT")<<" completed in ";
    out<<run.time.count()<<" seconds"<<std::endl;
    if (!run.complete) {
      out<<"Vertices left unprocessed: "<<run.last_i<<std::endl;
      out<<"Best found lower bound: "<<run.value<<std::endl;
    }
    else {
      out<<"Optimal solution size: "<<run.value<<std::endl;
    }
    if (run.certificate.empty()) {
      out<<"Vertices: {}"<<std::endl;
    }
    else {
      out<<"Vertices: {";
      for (size_t i = 0; i < run.certificate.size() - 1; ++i) {
        out<<run.certificate[i]<<", ";
      }
      out<<run.certificate.back()<<"}"<<std::endl;
      out<<"This solution is "<<(run.correct?"correct.":"INCORRECT!")<<std::endl;
    }
  }
  pr_();
}

void latex(std::ostream& out, const algorithm_run& run) {
  std::cout<<run.graphname<<"&";
  if (!run.valid) {
    out<<"FAIL\\\\"<<std::endl;
    return;
  }
  else {
    out<<(run.complete?std::string("OPT"):std::to_string(run.last_i))<<"&";
    out<<std::to_string(run.value)<<"&";
    out<<std::to_string(run.time.count())<<"\\\\"<<std::endl;
  }
}
}

#endif
