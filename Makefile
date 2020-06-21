export PKG_CONFIG_PATH = /opt/vc/lib/pkgconfig

CC = gcc
CXX = g++
CFLAGS = -Wall `sdl2-config --cflags` `pkg-config brcmglesv2 --cflags` `pkg-config SDL2_image --cflags` -I shader_loader
LIBS = `sdl2-config --libs` `pkg-config brcmglesv2 --libs` `pkg-config SDL2_image --libs`

shader_loader.o: shader_loader/shader_loader.c
	$(CC) $(CFLAGS) -c shader_loader/shader_loader.c -o shader_loader.o

opengles_fullscreen.o: opengles_fullscreen.cpp
	$(CXX) $(CFLAGS) -c opengles_fullscreen.cpp -o opengles_fullscreen.o

opengles_fullscreen: opengles_fullscreen.o shader_loader.o
	$(CXX) -o opengles_fullscreen opengles_fullscreen.o shader_loader.o $(LIBS)

.PHONY: clean test

clean:
	rm -f opengles_fullscreen *.o *~

test: opengles_fullscreen
	./opengles_fullscreen

