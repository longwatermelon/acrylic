CXX=g++
CXXFLAGS=-std=c++17 -ggdb -Wall

SRC=$(wildcard src/*.cpp)
OBJS=$(addprefix obj/, $(SRC:.cpp=.o))

.PHONY: dirs clean

all: dirs target

target: $(OBJS)
	$(CXX) $(CXXFLAGS) $^

obj/src/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

dirs:
	mkdir -p obj/src

clean:
	-rm -rf obj/

