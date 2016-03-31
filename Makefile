BINDIR := bin
OBJDIR := obj

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
OBJ := $(addprefix $(OBJDIR)/,$(notdir $(SRC:.cpp=.o)))

TEST_SRC := $(wildcard test/*.cpp)
TEST_OBJ := $(addprefix obj/,$(notdir $(TEST_SRC:.cpp=.o)))

all: $(BINDIR)/server $(BINDIR)/client $(BINDIR)/test

$(BINDIR)/server: $(OBJ) $(OBJDIR)/server.o
	@mkdir -p $(BINDIR)
	$(CXX) $(LDFLAGS) $^ -o $@

$(BINDIR)/client: $(OBJ) $(OBJDIR)/client.o
	@mkdir -p $(BINDIR)
	$(CXX) $(LDFLAGS) $^ -o $@

$(BINDIR)/test: $(TEST_OBJ)
	@mkdir -p $(BINDIR)
	$(CXX) $(LDFLAGS) $^ -o $@

$(OBJDIR)/server.o: src/server.cpp
	@mkdir -p obj
	$(CXX) $(CXXFLAGS) $(INC) $^ -c -o $@

$(OBJDIR)/client.o: src/client.cpp
	@mkdir -p obj
	$(CXX) $(CXXFLAGS) $(INC) $^ -c -o $@

$(OBJDIR)/%.o: src/%.cpp src/%.h
	@mkdir -p obj
	$(CXX) $(CXXFLAGS) $(INC) $< -c -o $@

$(OBJDIR)/%.o: test/%.cpp
	@mkdir -p obj
	$(CXX) $(CXXFLAGS) $(INC) $< -c -o $@

.PHONY: clean
clean:
	@rm -rf bin
	@rm -rf obj
