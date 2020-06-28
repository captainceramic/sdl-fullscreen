export PKG_CONFIG_PATH = /opt/vc/lib/pkgconfig
CC = gcc -Wall -std=gnu11
CFLAGS = `sdl2-config --cflags` -I ../shader_loader
LDFLAGS =  `sdl2-config --libs` `pkg-config brcmglesv2 --libs`


lighting_test.o: main.c object_loader.h
	$(CC) ${CFLAGS} -o lighting_test.o -c main.c

shader_loader.o: ../shader_loader/shader_loader.c
	$(CC) ${CFLAGS} -o shader_loader.o -c ../shader_loader/shader_loader.c

lighting_test: lighting_test.o shader_loader.o
	$(CC) ${LDFLAGS} -o lighting_test lighting_test.o shader_loader.o

.PHONY: all clean

test: lighting_test
	./lighting_test

clean:
	rm -rf *.o *~ lighting_test

