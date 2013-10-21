CFLAGS=-c -std=c++11 -g
LDFLAGS=-lpthread -lboost_thread-mt -lboost_system-mt

all: fakesource

fakesource: fakesource.o
	g++ -o fakesource $(LDFLAGS) fakesource.o

fakesource.o: fakesource.cpp
	g++ $(CFLAGS) fakesource.cpp

clean:
	rm *.o
