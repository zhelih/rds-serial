ifeq (${USE_OPENMP}, 1)
	CXXFLAGS+=-DUSE_OPENMP
	CXXFLAGS+=-fopenmp
endif

GRAPH_TYPE = GRAPH_MATRIX
BUILD_TYPE = O3

all: RDS tester

RDS: main.cpp verifiers/verifiers.h verifiers/*.hpp graph/*.hpp graph/*.cpp graph/*.h rds/*.hpp
	$(CXX) graph/graph.cpp graph/graph_adjacency.cpp graph/graph_matrix.cpp graph/orders.cpp main.cpp -lm -o RDS -Wall -Wextra -std=c++11 -Wno-unused-parameter -fopenmp -funroll-loops -fomit-frame-pointer -D$(GRAPH_TYPE) -$(BUILD_TYPE)

tester: tester.cpp
	$(CXX) tester.cpp -O2 -o tester -Wall -Wextra -std=c++11

clean:
	rm ./RDS
	rm ./tester
