ifeq (${USE_OPENMP}, 1)
	CXXFLAGS+=-DUSE_OPENMP
	CXXFLAGS+=-fopenmp
endif

all: rds tester

rds: graph.cpp rds.cpp main.cpp
	$(CXX) graph.cpp rds.cpp main.cpp -O2 -lm -o rds -Wall -Wextra -std=c++11 -Wno-unused-parameter -fopenmp -g

tester: tester.cpp
	$(CXX) tester.cpp -O2 -o tester -Wall -Wextra -std=c++11

clean:
	rm ./rds
	rm ./tester
