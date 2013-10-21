CFLAGS=-c -std=c++11 -g
LDFLAGS=-lpthread -lboost_thread-mt -lboost_system-mt

all: position_server

position_server: position-server.cc
	g++ -o position-server -Wall -ansi -pedantic -O3 position-server.cc -lpthread -lboost_thread-mt -lboost_system-mt -std=c++11

position_server_mac: position-server.cc
	clang++ -o position-server -Wall -ansi -pedantic -O3 position-server.cc -lpthread -lboost_thread-mt -lboost_system-mt -std=c++11 -stdlib=libc++ -I /opt/local/include -L /opt/local/lib

clean:
	rm position-server
