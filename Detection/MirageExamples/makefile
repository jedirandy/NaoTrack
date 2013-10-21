CC=g++
CFLAGS=-O3 `pkg-config --cflags mirage` 
LDFLAGS=`pkg-config --libs mirage`

TARGETS=example_001 example_002 example_003 example_004 example_005 example_006 example_007 example_008

all: $(TARGETS)

$(TARGETS): %: %.cpp
	$(CC) -o $@ $(CFLAGS) $< $(LDFLAGS)

clean:
	rm -f *.jpg *.ppm $(TARGETS)

clean_image:
	rm -f *.jpg *.ppm
