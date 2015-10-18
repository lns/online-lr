VERSION = 1

CXX = g++
CXXFLAGS = -Wall -std=c++0x -O2 -pthread -lrt -g -O0

INCLUDES = ../../src/

LIBS = ../../src/headers/*.hpp ../../src/learner/*.hpp ../../src/feeder/rtb2a/*.hpp

all: main

main: $(LIBS) main.cpp
	$(CXX) $(CXXFLAGS) main.cpp -I$(INCLUDES) -o main

clean:
	-rm -f main

.PHONY: clean

