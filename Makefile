
CFLAGS  = -g -Wall  -O2
#CFLAGS += -DPROXY_DEBUG
LDFLAGS =

all: HttpProxy
	./HttpProxy

HttpProxy: $(patsubst %.cpp, %.o, $(wildcard *.cpp))
	g++ $(CFLAGS) $(LDFLAGS) -o $@ $^

%.o: %.cpp
	g++ $(CFLAGS) $(LDFLAGS) -c -o $@ $^

clean:
	rm -rf HttpProxy *.o
