CC=g++
CFLAGS=-O3 `pkg-config --cflags mirage` 
LDFLAGS=`pkg-config --libs mirage`

TARGETS=color labelizer

all: $(TARGETS)

$(TARGETS): %: %.cpp
	$(CC) -o $@ $(CFLAGS) $< $(LDFLAGS)

clean:
	rm -f $(TARGETS)
