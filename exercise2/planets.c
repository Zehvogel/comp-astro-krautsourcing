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

#ifdef DOUBLE_PRECISION
typedef double f_t;
#else
typedef float f_t;
#endif

const f_t PI = 3.14159265358979323846;
const f_t G = 1.184e-4; /* in reduced units */

struct bodies {
	f_t *M;
	f_t *x;
	f_t *y;
	f_t *dx;
	f_t *dy;
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
	planets->dy[1] = -2*PI;

	/* Jupiter */
	if (M == 3) {
		planets->M[2] = 317.8;
		planets->x[2] = 5.0;
		planets->y[2] = 0.0;
		planets->dx[2] = 0.0;
		planets->dy[2] = -2*PI*0.42;
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

// maybe later
//static inline f_t velocity_verlet(f_t *x_1, f_t x_2, f_t m_2, f_t *dx_1, f_t dt)
//{
//}

int main(int argc, char *argv[])
{
	loop_t M  = 3;
	loop_t t  = 150;
	float dt = .0026;

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

	int file = open("planets.bin", O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWRITE);
	if (file == - 1)
		return errno;

	struct bodies planets;

	init_planets(&planets, M);

	f_t force_buffer[2 * M];
	memset(force_buffer, 0, sizeof(force_buffer));
#ifdef USE_VELOCITY_VERLET
	f_t old_force_buffer[2 * M];
	memset(old_force_buffer, 0, sizeof(force_buffer));
#endif

	for (loop_t i = 0; i * dt < t; i++) {
		for (loop_t j = 0; j < M; j++) {
			for (loop_t k = j + 1; k < M; k++) {
				f_t r_x = planets.x[j] - planets.x[k];
				f_t r_y = planets.y[j] - planets.y[k];
				f_t r_sqr = r_x * r_x + r_y * r_y;
				f_t r = sqrt(r_sqr);

				f_t a = -G / r_sqr;

				force_buffer[2*j] += a * r_x / r * planets.M[k];
				force_buffer[2*j+1] += a * r_y / r * planets.M[k];
				force_buffer[2*k] -= a * r_x / r * planets.M[j];
				force_buffer[2*k+1] -= a * r_y / r * planets.M[j];
			}
#ifdef USE_VELOCITY_VERLET
			//given x(t), v(t)
			//calculate a(t)
			//
			//for vv now do
			//x(t+1) = x(t) + v(t)dt + 0.5a(t)dt^2
			//calc a(t+1)
			//v(t+1) = v(t) + 0.5 * (a(t) + a(t+1)) * dt

			//might still be wrong but best working version yet

			//calc initial a
			if (i != 0) {
			planets.dx[j] += 0.5 * (force_buffer[2*j]
						+ old_force_buffer[2*j]) * dt;
			planets.dy[j] += 0.5 * (force_buffer[2*j+1]
						+ old_force_buffer[2*j+1]) * dt;
			planets.x[j] += planets.dx[j] * dt
					+ 0.5 * force_buffer[2*j] * dt * dt;
			planets.y[j] += planets.dy[j] * dt
					+ 0.5 * force_buffer[2*j+1] * dt * dt;
			}
#else
			// explicit euler
			// wait this isn't explicit euler
			// this is semi-implicit euler
			planets.dx[j] += dt * force_buffer[2*j];
			planets.dy[j] += dt * force_buffer[2*j+1];
			planets.x[j] += planets.dx[j] * dt;
			planets.y[j] += planets.dy[j] * dt;
#endif


			/* write output */
			pwrite(file, &planets.x[j], sizeof(planets.x[j]),
			       (i * 3 * M + 3*j) *sizeof(planets.x[j]));
			pwrite(file, &planets.y[j], sizeof(planets.y[j]),
			       (i * 3 * M + 3*j+1) *sizeof(planets.y[j]));

			f_t L_z = planets.M[j] * (planets.x[j] * planets.dy[j]
						 -planets.y[j] * planets.dx[j]);

			pwrite(file, &L_z, sizeof(L_z),
			       (i * 3 * M + 3*j+2) *sizeof(L_z));
		}
#ifdef USE_VELOCITY_VERLET
		memcpy(old_force_buffer, force_buffer, sizeof(force_buffer));
#endif
		memset(force_buffer, 0, sizeof(force_buffer));
	}

	free_planets(&planets);
	close(file);

	return EXIT_SUCCESS;
}
