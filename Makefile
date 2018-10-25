ifeq (${USE_OPENMP}, 1)
	CXXFLAGS+=-DUSE_OPENMP
	CXXFLAGS+=-fopenmp
endif

all: rds tester

rds: rds.cpp main.cpp verifiers/verifiers.h verifiers/*.hpp graph/*.hpp graph/*.cpp graph/*.h
	$(CXX) graph/graph.cpp graph/graph_adjacency.cpp graph/graph_matrix.cpp graph/orders.cpp rds.cpp main.cpp -O3 -lm -o rds -Wall -Wextra -std=c++11 -Wno-unused-parameter -fopenmp

tester: tester.cpp
	$(CXX) tester.cpp -O2 -o tester -Wall -Wextra -std=c++11

clean:
	rm ./rds
	rm ./tester
