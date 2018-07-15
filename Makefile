all:
	$(CXX) graph.cpp verifier.cpp rds.cpp main.cpp -O2 -lm -o rds -Wall -std=c++11
	$(CXX) tester.cpp -O2 -o tester -Wall -std=c++11
