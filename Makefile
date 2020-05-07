export PKG_CONFIG_PATH = /opt/vc/lib/pkgconfig

CXX = g++
CFLAGS = -Wall `sdl2-config --cflags` `pkg-config brcmglesv2 --cflags`
LDFLAGS = `sdl2-config --libs` `pkg-config brcmglesv2 --libs`

opengles_fullscreen.o: opengles_fullscreen.cpp
	$(CXX) $(CFLAGS) -c opengles_fullscreen.cpp -o opengles_fullscreen.o

opengles_fullscreen: opengles_fullscreen.o
	$(CXX) opengles_fullscreen.o $(LDFLAGS) -o opengles_fullscreen

.PHONY: clean test

clean:
	rm -f opengles_fullscreen *.o *~

test: opengles_fullscreen
	./opengles_fullscreen

