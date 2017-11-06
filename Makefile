CFLAGS=-std=c++11 -Wall -g 

CFLAGS+= $(shell pkg-config --cflags opencv)
LDFLAGS+= $(shell pkg-config --libs opencv)

demo: demo.o
	$(CXX) -O3 -o demo demo.o $(LDFLAGS)

demo.o: demo.cpp
	$(CXX) -O3 -c demo.cpp ${CFLAGS}
	
clean: 
	rm -f *.d *.o
	rm -f demo

