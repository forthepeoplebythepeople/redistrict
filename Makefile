# A GNU-style makefile

CPP = g++ -Wall -O3

.PHONY: all
all:  make_adjacency

make_adjacency: make_adjacency.cpp structures.hpp
	$(CPP) $(filter %.cpp,$^) -o $@


