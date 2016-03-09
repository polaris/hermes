CXX := clang

CXXFLAGS := -Wall -Wextra -pedantic -std=c++14 -O3
LDFLAGS := -lstdc++ -lm -lsdl2
INC :=

SRC := $(wildcard src/*.cpp)
OBJ := $(addprefix obj/,$(notdir $(SRC:.cpp=.o)))

all: bin/demo

bin/demo: $(OBJ)
	@mkdir -p bin
	$(CXX) $(LDFLAGS) $^ -o $@

obj/main.o: src/main.cpp
	@mkdir -p obj
	$(CXX) $(CXXFLAGS) $(INC) $^ -c -o $@

obj/%.o: src/%.cpp src/%.h
	@mkdir -p obj
	$(CXX) $(CXXFLAGS) $(INC) $< -c -o $@

.PHONY: clean
clean:
	@rm -rf bin
	@rm -rf obj
