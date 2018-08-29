This is the manual for Linux/UNIX/MacOS systems.
For windows please use the program parameters as specified for your IDE.

To compile the program run

make

or to compile with OpenMP

make WITH_OPENMP=1

To run the program and see input arguments run

./rds

DIMACS file format

p edge <V> <E>
e <from> <to>

example for a clique on 4 vertices

p edge 4 6
e 1 2
e 1 3
e 1 4
e 2 3
e 2 4
e 3 4

weights file containts lines where line i is the weight for vertex i
weights are INTEGERS!

example for a clique on for vertices where vertex i has weight i*i

1
4
9
16

Some examples for progrm launching. graph file example.dimacs weights file is example.weights

Find maximum clique
./rds -c example.dimacs

Find maximum weighted clique
./rds -c -weights example.weights example.dimacs

Find maximum clique with timelimit 10 minutes
./rds -c -t 600 example.dimacs
