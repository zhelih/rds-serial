#ifndef _UTILS_HPP
#define _UTILS_HPP
#include <vector>
#include "verifiers/verifiers.h"
#include <cstdio>

void pr_()
{
  printf("------------------------------------------------\n");
}

void verify_solution(verifier*& v, std::vector<uint>& sol) {
  printf("Solution verification: ");
  if(v->check_solution(sol))
    printf("CORRECT\n");
  else
    printf("INCORRECT\n");
}

class Line: public std::string { 
  friend std::istream & operator>>(std::istream & is, Line & line) { 
    return std::getline(is, line);
  }
};

#endif
