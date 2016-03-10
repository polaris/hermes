CXX := clang

CXXFLAGS := -Wall -Wextra -Wcast-align -Wcast-qual -Wctor-dtor-privacy \
            -Wdisabled-optimization -Wformat=2 -Winit-self \
            -Wmissing-declarations -Wmissing-include-dirs \
            -Wold-style-cast -Woverloaded-virtual -Wredundant-decls \
            -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-overflow=5 \
            -Wswitch-default -Wundef -Werror -Wno-unused -Wconversion \
            -Wsign-conversion -Weffc++ -pedantic -std=c++14 -O3
LDFLAGS := -lstdc++ -lm -lsdl2 -lsdl2_image -lboost_system
INC :=

MAIN := src/client.cpp src/server.cpp
SRC := $(filter-out $(addsuffix %,$(MAIN)),$(wildcard src/*.cpp))
OBJ := $(addprefix obj/,$(notdir $(SRC:.cpp=.o)))

all: bin/server bin/client

bin/server: $(OBJ) obj/server.o
	@mkdir -p bin
	$(CXX) $(LDFLAGS) $^ -o $@

bin/client: $(OBJ) obj/client.o
	@mkdir -p bin
	$(CXX) $(LDFLAGS) $^ -o $@

obj/server.o: src/server.cpp
	@mkdir -p obj
	$(CXX) $(CXXFLAGS) $(INC) $^ -c -o $@

obj/client.o: src/client.cpp
	@mkdir -p obj
	$(CXX) $(CXXFLAGS) $(INC) $^ -c -o $@

obj/%.o: src/%.cpp src/%.h
	@mkdir -p obj
	$(CXX) $(CXXFLAGS) $(INC) $< -c -o $@

.PHONY: clean
clean:
	@rm -rf bin
	@rm -rf obj
