.PHONY: all clean

CC=gfortran
FCFLAGS=-Wall -Werror -O3 -march=native -fopenmp -fopenacc

TARGETS=01_henlo_wurld 02_vec_mult_debug 02_vec_mult_vec 02_vec_mult_omp 02_vec_mult_acc

all: $(TARGETS)

02_vec_mult_debug: 02_vec_mult.f95
	${CC} -o $@ $^ -Wall -Werror -O0 -Og -g

02_vec_mult_vec: 02_vec_mult.f95
	${CC} -o $@ $^ -Wall -Werror -O0 -Og -g -ftree-vectorize

02_vec_mult_omp: 02_vec_mult.f95
	${CC} -o $@ $^ -Wall -Werror -O0 -Og -g -ftree-vectorize -fopenmp

02_vec_mult_acc: 02_vec_mult.f95
	${CC} -o $@ $^ -Wall -Werror -O0 -Og -g -ftree-vectorize -fopenacc

%:%.f95
	${CC} -o $@ $^ ${FCFLAGS}

clean:
	rm -f $(TARGETS)
