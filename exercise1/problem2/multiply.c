#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/time.h>
#include <cblas.h>


typedef signed long loop_t;
typedef double Matrix_t;

static void init(Matrix_t *M, loop_t n, Matrix_t l, Matrix_t d, Matrix_t r)
{
	for (loop_t i = 0; i < n; i++) {
		for (loop_t j = 0; j < n; j++) {
			if (i < j) {
				M[i*n + j] = r;
			} else if (i > j) {
				M[i*n + j] = l;
			} else {
				M[i*n + j] = d;
			}
		}
	}
}

static void init_A(Matrix_t *a, loop_t n)
{
	init(a, n, 5.0, 2.0, 4.0);
}

static void init_B(Matrix_t *b, loop_t n)
{
	init(b, n, 3.0, 1.0, 6.0);
}

static void print_matrix(Matrix_t *M, loop_t n)
{
	if (n <= 10) {
		for (loop_t i = 0; i < n; i++) {
			printf("\n");
			for (loop_t j = 0; j < n; j++) {
				printf("%.1f ", M[i*n + j]);
			}
			printf("\n");
		}
	}
}

int main(int argc, char *argv[])
{
	/* introduced iterations to achieve measurable execution times */
	loop_t n;

	switch (argc) {
		case 1:
			n = 5;
			break;
		case 2:
			if (!sscanf(argv[1], "%lu", &n))
				return EINVAL;
			break;
		default:
			fprintf(stderr, "Illegal parameter count\n");
			return EINVAL;
	}

	Matrix_t *restrict a, *restrict b, *restrict c;

	a = malloc(n * n * sizeof(*a));
	if (!a)
		return ENOMEM;

	b = malloc(n * n * sizeof(*b));
	if (!b) {
		free(a);
		return ENOMEM;
	}

	c = malloc(n * n * sizeof(*c));
	if (!c) {
		free(a);
		free(b);
		return ENOMEM;
	}

	init_A(a, n);
	init_B(b, n);

	printf("\nA:\n");
	print_matrix(a, n);

	printf("\nB:\n");
	print_matrix(b, n);
	struct timeval before;
	struct timeval after;

	/* slow */
//	memset(c, 0, n * n * sizeof(*c));
//	gettimeofday(&before, NULL);
//	/* Main löps */
//	for (loop_t i = 0; i < n; i++)
//		for (loop_t j = 0; j < n; j++)
//			for (loop_t k = 0; k < n; k++)
//				c[i*n + j] += a[i*n + k] * b[k*n + j];
//	gettimeofday(&after, NULL);
//	double serial_time = (after.tv_sec - before.tv_sec)
//			+ (after.tv_usec - before.tv_usec) * 1e-6;
//	printf("serial time: %.2f\n", serial_time);

	memset(c, 0, n * n * sizeof(*c));

	gettimeofday(&before, NULL);
	/* Main löps */
#pragma omp parallel for collapse(2)
	for (loop_t i = 0; i < n; i++)
		for (loop_t j = 0; j < n; j++)
			for (loop_t k = 0; k < n; k++)
				c[i*n + j] += a[i*n + k] * b[k*n + j];
	gettimeofday(&after, NULL);
	double omp_time = (after.tv_sec - before.tv_sec)
			+ (after.tv_usec - before.tv_usec) * 1e-6;
	printf("omp time: %.2f\n", omp_time);

	gettimeofday(&before, NULL);
	cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, n, n, n, 1.0, a,
			n, b, n, 0.0, c, n);
	gettimeofday(&after, NULL);
	double blas_time = (after.tv_sec - before.tv_sec)
			+ (after.tv_usec - before.tv_usec) * 1e-6;
	printf("cblas time: %.2f\n", blas_time);
	printf("\nC:\n");
	print_matrix(c, n);

	free(a);
	free(b);
	free(c);
	return EXIT_SUCCESS;
}
