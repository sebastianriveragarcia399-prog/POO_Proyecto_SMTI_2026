# Makefile - Proyecto SMTI (Linux / GCC).  Uso:  make   |   make run   |   make clean
CXX      := g++
CXXFLAGS := -std=c++14 -Wall -Wextra -g -Iinclude -pthread
BIN      := bin

SRCS := $(wildcard src/*.cpp)
OBJS := $(patsubst src/%.cpp,$(BIN)/%.o,$(SRCS))

all: $(BIN)/SMTI $(BIN)/cliente_sensor

$(BIN)/SMTI: $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(BIN)/%.o: src/%.cpp | $(BIN)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BIN)/cliente_sensor: clientes/cliente_sensor.cpp | $(BIN)
	$(CXX) $(CXXFLAGS) -o $@ $<

$(BIN):
	mkdir -p $(BIN)

run: all
	./$(BIN)/SMTI 3

clean:
	rm -rf $(BIN)

.PHONY: all run clean
