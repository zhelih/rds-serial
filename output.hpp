#ifndef _OUTPUT_HPP
#define _OUTPUT_HPP
#include <iostream>
#include "rds.h"
#include "utils.hpp"

namespace output
{
void fancy(std::ostream& out, const algorithm_run& run) {
  pr_();
  out<<"RDS run for graph "<<run.graphname;
  if (!run.valid) {
    out<<" failed."<<std::endl;
    return;
  }
  else {
    out<<" was"<<(run.complete?"":" not")<<" completed in ";
    out<<run.time.count()<<" seconds"<<std::endl;
    if (!run.complete) {
      out<<"Vertices left unprocessed: "<<run.last_i<<std::endl;
      out<<"Best found lower bound: "<<run.value<<std::endl;
    }
    else {
      out<<"Optimal solution size: "<<run.value<<std::endl;
    }
    std::vector<unsigned int> vertices = run.certificate;
    run.g->restore_order(vertices);
    std::sort(vertices.begin(), vertices.end());
    out<<"Vertices: {";
    for (unsigned int v: vertices) {
      out<<v<<", ";
    }
    out<<"\b\b}"<<std::endl;
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
