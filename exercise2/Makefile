.PHONY: all clean

CC=gcc
CFLAGS=-ggdb -Wall -Werror -O3 -march=native #-DOUBLE_PRECISION
LDFLAGS=-lm -lraylib

all: planets

planets: planets.o
	${CC} -o $@ $^ ${CFLAGS} ${LDFLAGS}

planets.pdf: planets.plt planets
	./planets -o file
	gnuplot planets.plt

clean:
	rm -f *.o planets planets.bin

