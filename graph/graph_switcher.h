#ifndef __GRAPH_SWITCHER_H__
#define __GRAPH_SWITCHER_H__

#ifdef GRAPH_MATRIX
#include "graph_matrix.h"
using Graph = graph_matrix;
#endif

#ifdef GRAPH_LIST
#include "graph_list.h"
using Graph = graph_adjacency;
#endif

#ifdef GRAPH_SET
#include "graph_cliquer.h"
using Graph = graph_cliquer;
#endif
#endif
