CXX := g++
CXXFLAGS := -I./include -m64 -mavx2 --std=c++14 -O3 -pthread

BIN_DIR := .
BINS := $(wildcard $(BIN_DIR)/*.cpp)
BIN_OBJS := $(BINS:%.cpp=%.o)
EXECS := $(BINS:%.cpp=%)

SRC_DIR := ./src
SRCS := $(wildcard $(SRC_DIR)/*.cpp)
SRC_OBJS := $(SRCS:%.cpp=%.o)

all: $(EXECS)

$(EXECS): $(SRC_OBJS) $(BIN_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $@.o $(SRC_OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f $(EXECS) $(SRC_OBJS) $(BIN_OBJS)

.PHONY: clean