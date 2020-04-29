.PHONY: all clean

CC=gcc
CFLAGS=-ggdb -Wall -Werror -Og -march=native# -fsanitize=address -lm # -fopenmp -L${MKLROOT}/lib/intel64 -lmkl_intel_ilp64 -lmkl_sequential -lmkl_core -lpthread -lm -ldl

all: planets plots.pdf

planets: planets.o
	${CC} -o $@ $^ ${CFLAGS}

plots.pdf: planets.plt 
	./planets
	gnuplot $<

clean:
	rm -f *.o planets planets.bin

