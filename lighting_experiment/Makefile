export PKG_CONFIG_PATH = /opt/vc/lib/pkgconfig
CC = gcc -Wall -std=gnu11
CFLAGS = `sdl2-config --cflags`
LDFLAGS =  `sdl2-config --libs` `pkg-config brcmglesv2 --libs`


lighting_test.o: main.c object_loader.h
	$(CC) ${CFLAGS} -o lighting_test.o -c main.c

lighting_test: lighting_test.o
	$(CC) ${LDFLAGS} -o lighting_test lighting_test.o

.PHONY: all clean

test: lighting_test
	./lighting_test

clean:
	rm -rf *.o *~ lighting_test

