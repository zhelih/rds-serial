#ifndef _RDS_H
#define _RDS_H

#include <vector>
#include "graph.h"
#include "verifier.h"

typedef unsigned int uint;

uint rds(verifier* v, graph*g, std::vector<uint>& res);
void print_lb_atomic(int signal);

#endif
