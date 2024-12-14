Russian Doll Search for Computing Maximum Vertex Weight Hereditary Structures in Graphs

Copyright Eugene Lykhovyd, 2017-2018.
Copyright Mykyta Makovenko, 2018.

See LICENSE for more details.

A graph property is hereditary on induced subgraphs if it holds for all vertex-induced subgraphs of
a graph satisfying this property. Given a fixed nontrivial, hereditary graph property and a
vertex-weighted graph, we consider the problem of finding the maximum weight subset of vertices
inducing a subgraph satisfying this property. Russian Doll Search provides an attractive framework
for solving this class of problems. We develop a C++ implementation of this approach, which can be
used to find maximum vertex weight structures for any hereditary property, as long as one can
provide an efficient feasibility verification procedure for a subgraph obtained from a feasible
subgraph by adding a new vertex.

See https://wp.lykhovyd.com/rds/ for more details.

[![Build](https://github.com/zhelih/rds-serial/actions/workflows/build.yml/badge.svg)](https://github.com/zhelih/rds-serial/actions/workflows/build.yml)
