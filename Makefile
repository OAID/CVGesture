CFLAGS=-std=c++11 -Wall -g 

export PKG_CONFIG_PATH=/usr/local/AID/pkgconfig

CFLAGS+= `pkg-config --cflags opencv`
LDFLAGS+= `pkg-config --libs opencv`

demo: demo.o gesture.o
	$(CXX) -O3 -o demo demo.o gesture.o $(LDFLAGS)

demo.o: demo.cpp
	$(CXX) -O3 -c demo.cpp ${CFLAGS}

gesture.o: gesture.cpp
	$(CXX) -O3 -c gesture.cpp ${CFLAGS}
	
clean: 
	rm -f *.d *.o
	rm -f demo

