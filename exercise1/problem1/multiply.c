#include <stdlib.h>
#include <stdio.h>
#include <errno.h>


typedef signed long loop_t;

int main(int argc, char *argv[])
{
	/* introduced iterations to achieve measurable execution times */
	loop_t n, iterations = 10;

	switch (argc) {
		case 1:
			n = 1000;
			break;
		case 2:
			if (!sscanf(argv[1], "%lu", &n))
				return EINVAL;
			break;
		case 3:
			if (!(sscanf(argv[1], "%lu", &n) &&
					sscanf(argv[2], "%lu", &iterations)))
				return EINVAL;
			break;
		default:
			fprintf(stderr, "Illegal parameter count\n");
			return EINVAL;
	}

	double *restrict a, *restrict b, *restrict c;

	a = malloc(n * sizeof(*a));
	if (!a)
		return ENOMEM;

	b = malloc(n * sizeof(*b));
	if (!b) {
		free(a);
		return ENOMEM;
	}

	c = malloc(n * sizeof(*c));
	if (!c) {
		free(a);
		free(b);
		return ENOMEM;
	}

	/* Init vectors */
	for (loop_t i = 0; i < n; i++)
		a[i] = b[n-i-1] = i;

	/* Main löp */
	for (loop_t i = 0; i < iterations; i++)
#pragma omp parallel for
		for (loop_t j = 0; j < n; j++)
			c[j] = a[j] * b[j];


	/* use c so that O2 and friends don't just delete the whole loop */
	printf("c[0] = %f\n", c[0]);

	free(a);
	free(b);
	free(c);
	return EXIT_SUCCESS;
}
