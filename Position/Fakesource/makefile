CFLAGS=-c -std=c++11 -g
LDFLAGS=-lpthread -lboost_thread-mt -lboost_system-mt

all: fakesource

fakesource: fakesource.o
	g++ -o fakesource $(LDFLAGS) fakesource.o

fakesource.o: fakesource.cpp
	g++ $(CFLAGS) fakesource.cpp

fakesource_mac: fakesource.cpp
	clang++ -o fakesource -Wall -ansi -pedantic -O3 fakesource.cpp $(LDFLAGS) -std=c++11 -stdlib=libc++ -I /opt/local/include -L /opt/local/lib

clean:
	rm *.o
