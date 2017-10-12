CFLAGS=-std=c++11 -Wall -g 

CFLAGS+= -I/usr/local/include
LDFLAGS+=-L/usr/local/lib  -lopencv_videoio -lopencv_video -lopencv_imgproc -lopencv_core -lopencv_imgcodecs -lopencv_highgui -lopencv_objdetect

demo: demo.o
	$(CXX) -O3 -o demo demo.o $(LDFLAGS)

demo.o: demo.cpp
	$(CXX) -O3 -c demo.cpp ${CFLAGS}
	
clean: 
	rm -f *.d *.o
	rm -f demo

