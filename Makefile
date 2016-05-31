BINDIR := bin
OBJDIR := obj

CXXFLAGS := -Wall -Wextra -Wcast-align -Wcast-qual -Wctor-dtor-privacy \
            -Wdisabled-optimization -Wformat=2 -Winit-self \
            -Wmissing-declarations -Wmissing-include-dirs \
            -Wold-style-cast -Woverloaded-virtual -Wredundant-decls \
            -Wshadow -Wsign-conversion -Wsign-promo \
            -Wswitch-default -Wundef -Werror -Wno-unused -Wconversion \
            -Wsign-conversion -Weffc++ -pedantic -std=c++14 -O3
LDFLAGS := -lstdc++ -lm -lboost_system

OS_NAME := $(shell uname -s | tr A-Z a-z)

ifeq ($(OS_NAME),linux)
    LDFLAGS += -lSDL2 -lSDL2_image -lboost_thread -pthread
endif
ifeq ($(OS_NAME),darwin)
    LDFLAGS += -lsdl2 -lsdl2_image -lboost_thread-mt
endif

INC := -Isrc -isystem 3rdparty

MAIN := src/client.cpp src/server.cpp src/peer.cpp
SRC := $(filter-out $(addsuffix %,$(MAIN)),$(wildcard src/*.cpp))
OBJ := $(addprefix $(OBJDIR)/,$(notdir $(SRC:.cpp=.o)))

TEST_SRC := $(wildcard test/*.cpp)
TEST_OBJ := $(addprefix obj/,$(notdir $(TEST_SRC:.cpp=.o)))

all: $(BINDIR)/server $(BINDIR)/client $(BINDIR)/peer $(BINDIR)/test

$(BINDIR)/server: $(OBJ) $(OBJDIR)/server.o
	@mkdir -p $(BINDIR)
	$(CXX) $^ $(LDFLAGS) -o $@

$(OBJDIR)/server.o: src/server.cpp
	@mkdir -p obj
	$(CXX) $(CXXFLAGS) $(INC) $^ -c -o $@

$(BINDIR)/client: $(OBJ) $(OBJDIR)/client.o
	@mkdir -p $(BINDIR)
	$(CXX) $^ $(LDFLAGS) -o $@

$(OBJDIR)/client.o: src/client.cpp
	@mkdir -p obj
	$(CXX) $(CXXFLAGS) $(INC) $^ -c -o $@

$(BINDIR)/peer: $(OBJ) $(OBJDIR)/peer.o
	@mkdir -p $(BINDIR)
	$(CXX) $^ $(LDFLAGS) -o $@

$(OBJDIR)/peer.o: src/peer.cpp
	@mkdir -p obj
	$(CXX) $(CXXFLAGS) $(INC) $^ -c -o $@

$(BINDIR)/test: $(TEST_OBJ) $(OBJ)
	@mkdir -p $(BINDIR)
	$(CXX) $^ $(LDFLAGS) -o $@

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

.PHONY: check
check:
	@cppcheck --enable=all --std=c++11 --platform=native src