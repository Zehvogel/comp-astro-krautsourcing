.PHONY: all clean

CC=gcc
CFLAGS=-g -Wall -O3 -march=native -fopenmp -L${MKLROOT}/lib/intel64 -lmkl_intel_ilp64 -lmkl_sequential -lmkl_core -lpthread -lm -ldl

all: multiply

multiply:multiply.o
	${CC} -o $@ $^ ${CFLAGS}

clean:
	rm -f *.o

