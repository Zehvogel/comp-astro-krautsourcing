.PHONY: all clean

CC=gcc
CFLAGS=-ggdb -Wall -Werror -O3 -march=native -DUSE_RAYLIB #-DUSE_VELOCITY_VERLET # -DUSE_SYMPLECTIC_EULER #-DOUBLE_PRECISION # -fopenmp -L${MKLROOT}/lib/intel64 -lmkl_intel_ilp64 -lmkl_sequential -lmkl_core -lpthread -lm -ldl
LDFLAGS=-lm -lraylib

all: planets

planets: planets.o
	${CC} -o $@ $^ ${CFLAGS} ${LDFLAGS}

planets.pdf: planets.plt planets
	./planets
	gnuplot planets.plt

clean:
	rm -f *.o planets planets.bin

