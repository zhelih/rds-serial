ifeq (${USE_OPENMP}, 1)
	CXXFLAGS+=-DUSE_OPENMP
	CXXFLAGS+=-fopenmp
endif

# GRAPH_MATRIX, GRAPH_SET, GRAPH_LIST
GRAPH_TYPE = GRAPH_MATRIX
# O3 or g
BUILD_TYPE = O3

all: RDS tester

RDS: main.cpp verifiers/verifiers.h verifiers/*.hpp graph/*.hpp graph/*.cpp graph/*.h rds/*.hpp version.c
	$(CXX) graph/graph.cpp graph/graph_list.cpp graph/graph_matrix.cpp graph/orders.cpp version.c main.cpp -lm -o RDS -Wall -Wextra -std=c++11 -Wno-unused-parameter -fopenmp -funroll-loops -fomit-frame-pointer -D$(GRAPH_TYPE) -$(BUILD_TYPE)

tester: tester.cpp
	$(CXX) tester.cpp -O2 -o tester -Wall -Wextra -std=c++11

peeler: peeler.cpp graph/graph.cpp graph/graph_utils.hpp
	$(CXX) peeler.cpp -Wall -Wextra -O2 -o peeler -std=c++11

version.c: .git/HEAD .git/index
	echo "const char *gitversion = \"$(shell git describe --tags --always)\";" > $@

clean:
	rm ./RDS
	rm ./tester
	rm version.c
