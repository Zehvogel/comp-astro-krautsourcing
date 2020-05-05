.PHONY: all clean

CC=gcc
CFLAGS=-ggdb -Wall -Werror -O3 -march=native -DUSE_SYMPLECTIC_EULER #-DUSE_VELOCITY_VERLET #-DOUBLE_PRECISION # -fopenmp -L${MKLROOT}/lib/intel64 -lmkl_intel_ilp64 -lmkl_sequential -lmkl_core -lpthread -lm -ldl
LDFLAGS=-lm

all: planets planets.pdf

planets: planets.o
	${CC} -o $@ $^ ${CFLAGS} ${LDFLAGS}

planets.pdf: planets.plt planets
	./planets
	gnuplot planets.plt

clean:
	rm -f *.o planets planets.bin

