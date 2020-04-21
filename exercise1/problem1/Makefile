.PHONY: all clean

CC=gcc
CFLAGS=-g -Wall -Og#-O3 -march=native -fopenmp

all: multiply

multiply:multiply.o
	${CC} -o $@ $^ ${CFLAGS}

clean:
	rm -f *.o

