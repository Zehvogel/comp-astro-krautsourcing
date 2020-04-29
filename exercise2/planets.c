#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>
#include <string.h>

/*
 * TODO: recheck because i messed up somewhere
 * Dimension choices:
 * Length: AU
 * Mass: Earth masses
 * Time: Earth years
 * Velocity: AU / (2*PI earth years) to comply with the given init
 */

const float PI = 3.14159265358979323846;
const float G = 1.184e-4; /* in reduced units */

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

static inline float f(float x_1, float x_2, float dx, float y_1, float y_2)
{
	float r_x = x_1 - x_2;
	float r_y = y_1 - y_2;
	//FIXME: rethinkt the physics and stop dividing by zero uff
	return -G / (r_x * r_x) + dx * dx / (r_x * r_x + r_y * r_y);
}

static inline float explicit_euler(float f, float y, float h)
{
	return y + h * f;
}

// maybe later
//static inline float velocity_verlet(float *x_1, float x_2, float m_2, float *dx_1, float dt)
//{
//}

int main(int argc, char *argv[])
{
	loop_t M  = 2;
	loop_t t  = 1;
	float dt = 1.0/12;//.0027;

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
					sscanf(argv[2], "%f", &dt)))
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

	float force_buffer[2 * M];

	for (loop_t i = 0; i * dt < t; i++) {
		memset(force_buffer, 0, 2 * M * sizeof(*force_buffer));
		for (loop_t j = 0; j < M; j++) {
			for (loop_t k = j + 1; k < M; k++) {
				float a_x = f(planets.x[j], planets.x[k],
					      planets.dx[j], planets.y[j],
					      planets.y[k]);

				float a_y = f(planets.y[j], planets.y[k],
					      planets.dy[j], planets.x[j],
					      planets.x[k]);

				force_buffer[2*j] += a_x;
				force_buffer[2*j+1] += a_y;
				force_buffer[2*k] -= a_x;
				force_buffer[2*k+1] -= a_y;
			}
			planets.dx[j] = explicit_euler(force_buffer[2*j]
						       * planets.M[j],
						       planets.dx[j], dt);
			planets.dy[j] = explicit_euler(force_buffer[2*j+1]
						       * planets.M[j],
						       planets.dy[j], dt);
			planets.x[j] += planets.dx[j] * dt;
			planets.y[j] += planets.dy[j] * dt;

			pwrite(file, &planets.x[1], sizeof(planets.x[1]),
			       i * 2 *sizeof(planets.x[1]));
			pwrite(file, &planets.y[1], sizeof(planets.y[1]),
			       (i * 2 + 1) * sizeof(planets.y[1]));
			printf("it: %ld\t x: %f\t y: %f\n", i, planets.x[1],
					planets.y[1]);
		}
	}

	free_planets(&planets);

	return EXIT_SUCCESS;
}
