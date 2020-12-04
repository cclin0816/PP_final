CXX := g++
CXXFLAGS := -m64 -mavx2 --std=c++14 -O3
EXE := main
OBJS := main.o node.o game.o mcts.o

all: $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(EXE) $(OBJS)

%.o: %.c
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -rf *.o main

.PHONY: all