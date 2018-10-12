ifeq (${USE_OPENMP}, 1)
	CXXFLAGS+=-DUSE_OPENMP
	CXXFLAGS+=-fopenmp
endif

all: rds tester

rds: graph.cpp rds.cpp main.cpp verifiers/verifiers.h verifiers/*.hpp
	$(CXX) graph.cpp rds.cpp main.cpp -O2 -lm -o rds -Wall -Wextra -std=c++14 -Wno-unused-parameter -fopenmp -g

rds-debug: graph.cpp rds.cpp main.cpp
	$(CXX) graph.cpp rds.cpp main.cpp -g -lm -o rds -Wall -Wextra -std=c++14 -Wno-unused-parameter -fopenmp -g

tester: tester.cpp
	$(CXX) tester.cpp -O2 -o tester -Wall -Wextra -std=c++11

clean:
	rm ./rds
	rm ./tester
