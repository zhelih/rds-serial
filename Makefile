ifeq (${USE_OPENMP}, 1)
	CXXFLAGS+=-DUSE_OPENMP
	CXXFLAGS+=-fopenmp
endif

all: RDS_mat tester

RDS_adj: main.cpp verifiers/verifiers.h verifiers/*.hpp graph/*.hpp graph/*.cpp graph/*.h rds/*.hpp
	cp rds/rds_adjacency.hpp rds/rds.hpp
	cp rds/rds_utils_adjacency.hpp rds/rds_utils.hpp
	cp verifiers/verifier_adjacency.hpp verifiers/verifier.hpp
	$(CXX) graph/graph.cpp graph/graph_adjacency.cpp graph/graph_matrix.cpp graph/orders.cpp main.cpp -O3 -lm -o RDS -Wall -Wextra -std=c++11 -Wno-unused-parameter -fopenmp
	rm rds/rds.hpp
	rm rds/rds_utils.hpp
	rm verifiers/verifier.hpp

RDS_mat: main.cpp verifiers/verifiers.h verifiers/*.hpp graph/*.hpp graph/*.cpp graph/*.h rds/*.hpp
	cp rds/rds_matrix.hpp rds/rds.hpp
	cp rds/rds_utils_matrix.hpp rds/rds_utils.hpp
	cp verifiers/verifier_matrix.hpp verifiers/verifier.hpp
	$(CXX) graph/graph.cpp graph/graph_adjacency.cpp graph/graph_matrix.cpp graph/orders.cpp main.cpp -O3 -lm -o RDS -Wall -Wextra -std=c++11 -Wno-unused-parameter -fopenmp -funroll-loops -fomit-frame-pointer
	rm rds/rds.hpp
	rm rds/rds_utils.hpp
	rm verifiers/verifier.hpp

RDS_clq: main.cpp verifiers/verifiers.h verifiers/*.hpp graph/*.hpp graph/*.cpp graph/*.h rds/*.hpp
	cp rds/rds_cliquer.hpp rds/rds.hpp
	cp rds/rds_utils_cliquer.hpp rds/rds_utils.hpp
	cp verifiers/verifier_cliquer.hpp verifiers/verifier.hpp
	$(CXX) graph/graph.cpp graph/orders.cpp main.cpp -O3 -lm -o RDS -Wall -Wextra -std=c++11 -Wno-unused-parameter -fopenmp
	rm rds/rds.hpp
	rm rds/rds_utils.hpp
	rm verifiers/verifier.hpp	

RDS-debug: main.cpp verifiers/verifiers.h verifiers/*.hpp graph/*.hpp graph/*.cpp graph/*.h rds/*.hpp
	$(CXX) graph/graph.cpp graph/graph_adjacency.cpp graph/graph_matrix.cpp graph/orders.cpp main.cpp -g -lm -o RDSDebug -Wall -Wextra -std=c++11 -Wno-unused-parameter -fopenmp

tester: tester.cpp
	$(CXX) tester.cpp -O2 -o tester -Wall -Wextra -std=c++11

clean:
	rm ./RDS
	rm ./tester
