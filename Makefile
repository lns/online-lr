#
# To train your model, try
# 	$ ./main imp.list 60000000 0
#
VERSION = 1

CXX = g++
CXXFLAGS = -Wall -std=c++0x -O2 -pthread -lrt

OBJS =

all: main

main: $(OBJS) src/main.cpp src/*.hpp
	$(CXX) $(CXXFLAGS) $(OBJS) src/main.cpp -o main

debug.main: $(OBJS) src/main.cpp src/*.hpp
	$(CXX) $(CXXFLAGS) -O0 -g $(OBJS) src/main.cpp -o debug.main

doc:
	doxygen Doxyfile

clean: 
	-rm -f *.o
	-rm -f src/*.o
	-rm -f main
	-rm -f debug.main
	-rm -rf doc/

.PHONY: doc
