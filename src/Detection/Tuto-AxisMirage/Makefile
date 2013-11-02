CFLAGS = -Wall `pkg-config --cflags axisPTZ mirage`
LIBS   = `pkg-config --libs axisPTZ mirage`

all: tuto.bin usage

tuto.bin : tuto.cc
	g++ -o tuto.bin $(CFLAGS) tuto.cc $(LIBS)

clean :
	rm -f *~ tuto.bin ptz.jpg

usage :
	@echo
	@echo "Example of usage :"
	@echo   "./tuto.bin ptz1.grid.metz.supelec.fr 80 frezza gloubi -5 -18 3000"
	@echo



