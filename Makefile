TOPDIR =$(shell pwd)
CXX = g++
CXXFLAGS+= -std=c++11 -Wall -g

export PKG_CONFIG_PATH=/usr/local/AID/pkgconfig

CXXFLAGS+= `pkg-config --cflags opencv`
LDFLAGS+= `pkg-config --libs opencv`

CXXFLAGS += -I$(TOPDIR)/include

objs_list += demo.o
objs_list += gesture.o
objs_list += tools/jsoncpp.o
objs_list += tools/log.o
objs_list += tools/perf.o

EXES = demo

all: $(EXES)

$(EXES) : $(objs_list)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)
	
clean: 
	$(RM) $(EXES)
	$(RM) $(objs_list)

