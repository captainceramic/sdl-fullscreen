export PKG_CONFIG_PATH = /opt/vc/lib/pkgconfig

CC = gcc -Wall

CFLAGS = `pkg-config brcmegl --cflags`
LDFLAGS = `pkg-config brcmegl --libs`

egl_experiment: egl_experiment.o
	$(CC) $(LDFLAGS) -o egl_experiment egl_experiment.o

egl_experiment.o: egl_experiment.c
	$(CC) $(CFLAGS) -o egl_experiment.o -c egl_experiment.c

clean:
	rm -rf *.o egl_experiment *~

test: egl_experiment
	./egl_experiment

.PHONY: clean test

