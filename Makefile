all:
	g++ graph.cpp verifier.cpp rds.cpp main.cpp -O2 -lm -o rds -Wall -std=c++14
	g++ tester.cpp -O2 -o tester -Wall -std=c++11
