export PKG_CONFIG_PATH = /opt/vc/lib/pkgconfig

CC = gcc -Wall
CPP = g++ -Wall

CFLAGS = `sdl2-config --cflags` `pkg-config brcmglesv2 --cflags`
LIBS = `sdl2-config --libs` `pkg-config brcmglesv2 --libs`

shader_loader.o: ../shader_loader.c
	$(CC) $(CFLAGS) -c ../shader_loader.c -o shader_loader.o

teapot.o: teapot.cpp object_loader.hpp
	$(CPP) $(CFLAGS)  -c teapot.cpp -o teapot.o

teapot: shader_loader.o teapot.o
	$(CPP) $(LIBS) -o teapot teapot.o shader_loader.o

.PHONY: test clean

test: teapot
	./teapot

clean:
	rm -rf */*~ *~ teapot *.o
