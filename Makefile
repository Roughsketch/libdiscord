CXX=g++
CXXFLAGS=-DRAPIDJSON_HAS_STDSTRING -DELPP_DISABLE_DEBUG_LOGS -DELPP_DISABLE_TRACE_LOGS -Ilibdiscord/include -std=c++14 -O3 -c -fPIC
LDLIBS=-lboost_system -lcrypto -lssl -lcpprest -lz -lpthread
SRCS=$(wildcard libdiscord/src/*.cpp) $(wildcard libdiscord/src/api/*.cpp) $(wildcard libdiscord/src/event/*.cpp)
OBJS=$(subst .cpp,.o,$(SRCS))
LIB=lib/libdiscord.so

all: $(SRCS) $(LIB)

$(LIB): $(OBJS)
	$(CXX) $(OBJS) -shared -o $@

.cpp.o:
	$(CXX) $(CXXFLAGS) $< $(LDLIBS) -o $@

install:
	cp lib/libdiscord.so /usr/lib/ 

rebuild: clean all

clean:
	rm $(OBJS)
	rm $(LIB)

