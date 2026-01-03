CXX=g++
CXXFLAGS=-std=c++17 -Wall

SRC=src/main.cpp src/allocator/memory_manager.cpp
OUT=memsim

all:
	$(CXX) $(CXXFLAGS) $(SRC) -o $(OUT)

clean:
	del /Q memsim.exe || echo nothing to clean

