#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>

/*
 * Dimension choices:
 * Length: AU
 * Mass: Earth masses
 * Time: Earth years
 * Velocity: AU / (2*PI earth years) to comply with the given init
 */

const float PI = 3.14159265358979323846;
const float G = 1.184e-4 / (2 * PI); /* in reduced units */

struct bodies {
	float *M;
	float *x;
	float *y;
	float *dx;
	float *dy;
};

typedef signed long loop_t;

void init_planets(struct bodies *planets, loop_t M)
{
	planets->M = malloc(M * sizeof(*planets->M));
	planets->x = malloc(M * sizeof(*planets->x));
	planets->y = malloc(M * sizeof(*planets->y));
	planets->dx = malloc(M * sizeof(*planets->dx));
	planets->dy = malloc(M * sizeof(*planets->dy));

	/* Sun */
	planets->M[0] = 333e3;
	planets->x[0] = 0.0;
	planets->y[0] = 0.0;
	planets->dx[0] = 0.0;
	planets->dy[0] = 0.0;

	/* Earth */
	planets->M[1] = 1.0;
	planets->x[1] = 1.0;
	planets->y[1] = 0.0;
	planets->dx[1] = 0.0;
	planets->dy[1] = -1.0;

	/* Jupiter */
	if (M == 3) {
		planets->M[2] = 317.8;
		/* tbd */
	}
}

void free_planets(struct bodies *planets)
{
	free(planets->M);
	free(planets->x);
	free(planets->y);
	free(planets->dx);
	free(planets->dy);
}

static inline float f(float x_1, float x_2, float m_2, float dt)
{
	return G * m_2 * dt / ((x_1 - x_2) * (x_1 - x_2));
}

static inline float explicit_euler(float f)
{
	return 0.0;
}

int main(int argc, char *argv[])
{
	loop_t M  = 2;
	loop_t t  = 500;
	float delta_t = .1;

	switch (argc) {
		case 1:
			/* nothing to do */
			break;
		case 2:
			if (!sscanf(argv[1], "%lu", &t))
				return EINVAL;
			break;
		case 3:
			if (!(sscanf(argv[1], "%lu", &t) &&
					sscanf(argv[2], "%f", &delta_t)))
				return EINVAL;
			break;
		default:
			fprintf(stderr, "Illegal parameter count\n");
			return EINVAL;
	}

	int file = open("planets.bin", O_CREAT | O_WRONLY | O_TRUNC);
	if (file == - 1)
		return errno;

	struct bodies planets;

	init_planets(&planets, M);

	for (loop_t i = 0; i < t; i++) {
		// calc forces
		// update positions / velocities
		// dump positions / velocities to disk
	}



//	for (loop_t i = 0; i < t; i++) {
//		float x = cos(i * delta_t);
//		float y = sin(i * delta_t);
//		pwrite(file, &x, sizeof(x), i * (sizeof(x) + sizeof(y)));
//		pwrite(file, &y, sizeof(y), i * (sizeof(x) + sizeof(y)) + sizeof(x));
//	}


	free_planets(&planets);

	return EXIT_SUCCESS;
}
