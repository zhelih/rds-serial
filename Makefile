all:
	g++ graph.cpp verifier.cpp rds.cpp main.cpp -O2 -lm -o rds -Wall -std=c++14
