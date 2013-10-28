CC=g++
CFLAGS=-O3 `pkg-config --cflags mirage axisPTZ` 
LDFLAGS=`pkg-config --libs mirage axisPTZ`

TARGETS=color labelizer camera

all: $(TARGETS)

$(TARGETS): %: %.cpp
	$(CC) -o $@ $(CFLAGS) $< $(LDFLAGS)

clean:
	rm -f $(TARGETS)
