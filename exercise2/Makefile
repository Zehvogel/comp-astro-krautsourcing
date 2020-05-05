.PHONY: all clean

CC=gcc
CFLAGS=-g -Wall -Werror -Og -march=native -fsanitize=address -lm -DUSE_VELOCITY_VERLET #-DOUBLE_PRECISION # -fopenmp -L${MKLROOT}/lib/intel64 -lmkl_intel_ilp64 -lmkl_sequential -lmkl_core -lpthread -lm -ldl

all: planets planets.pdf

planets: planets.o
	${CC} -o $@ $^ ${CFLAGS}

planets.pdf: planets.plt planets
	./planets
	gnuplot planets.plt

clean:
	rm -f *.o planets planets.bin

