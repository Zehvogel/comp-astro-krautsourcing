#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>
#include <string.h>
#include <raylib.h>

/*
 * Dimension choices:
 * Length: AU
 * Mass: Earth masses
 * Time: Earth years
 * Velocity: AU / earth years
 */

#ifdef DOUBLE_PRECISION
typedef double f_t;
#else
typedef float f_t;
#endif

const f_t pi = 3.14159265358979323846;
const f_t G = 1.184e-4; /* in reduced units */

struct bodies {
	f_t *M;
	f_t *x;
	f_t *y;
	f_t *dx;
	f_t *dy;
};

enum output_type {
	RAYLIB,
	BINFILE
};

enum integrator {
	EXPLICIT_EULER,
	SEMI_IMPLICIT_EULER,
	VELOCITY_VERLET
};

typedef signed long loop_t;

static void init_planets(struct bodies *planets, loop_t M)
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
	planets->dy[1] = -2*pi;

	/* Jupiter */
	if (M == 3) {
		planets->M[2] = 317.8;
		planets->x[2] = 5.0;
		planets->y[2] = 0.0;
		planets->dx[2] = 0.0;
		planets->dy[2] = -2*pi*0.42;
	}
}

static void free_planets(struct bodies *planets)
{
	free(planets->M);
	free(planets->x);
	free(planets->y);
	free(planets->dx);
	free(planets->dy);
}

int main(int argc, char *argv[])
{
	/* needs more refactoring to work with M = 2 again */
	loop_t M  = 3;

	loop_t t  = 150;
	float dt = .0026;
	unsigned FPS = 60;
	loop_t frameskip = 1;
	enum integrator integrator = SEMI_IMPLICIT_EULER;
	enum output_type output = RAYLIB;
	int file = -1;

	int opt;
	while ((opt = getopt(argc, argv, "ht:d:f:s:o:i:")) != -1) {
		switch (opt) {
			case 't':
				if (sscanf(optarg, "%ld", &t))
					break;
				else
					exit(EINVAL);
			case 'd':
				if (sscanf(optarg, "%f", &dt))
					break;
				else
					exit(EINVAL);
			case 'f':
				if (sscanf(optarg, "%u", &FPS))
					break;
				else
					exit(EINVAL);
			case 's':
				if (sscanf(optarg, "%ld", &frameskip))
					break;
				else
					exit(EINVAL);
			case 'o':
				if (strcmp(optarg, "raylib") == 0) {
					output = RAYLIB;
					break;
				} else if (strcmp(optarg, "file") == 0) {
					output = BINFILE;
					break;
				} else {
					exit(EINVAL);
				}
			case 'i':
				if (strcmp(optarg, "EE") == 0) {
					integrator = EXPLICIT_EULER;
					break;
				} else if (strcmp(optarg, "SIE") == 0) {
					integrator = SEMI_IMPLICIT_EULER;
					break;
				} else if (strcmp(optarg, "VV") == 0) {
					integrator = VELOCITY_VERLET;
					break;
				} else {
					exit(EINVAL);
				}
			default: /* fall through */
			case 'h':
				printf("Usage: %s [-h] [-t years] [-d stepsize] [-f FPS] [-s frameskip] [-o output: raylib or file] [-i integrator: EE, SIE, VV]\n", argv[0]);
				exit(EXIT_FAILURE);
		}
	}

	if (output == RAYLIB) {
		InitWindow(800, 600, "Planets");
		SetTargetFPS(FPS);
	}
	if (output == BINFILE) {
		file = open("planets.bin", O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWRITE);
		if (file == - 1)
			exit(errno);
	}

	struct bodies planets;

	init_planets(&planets, M);

	f_t force_buffer[2 * M];
	memset(force_buffer, 0, sizeof(force_buffer));

	/* XXX: only needed for VV but M is so small that I don't care */
	f_t old_force_buffer[2 * M];
	memset(old_force_buffer, 0, sizeof(force_buffer));

	loop_t i = 0;
	while (1 == 1) {
		f_t E = 0;
		for (loop_t j = 0; j < M; j++) {
			/* TODO: refactor */
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

				/* add potential energy */
				E += G / r * (planets.M[j] + planets.M[k]);
			}
			/* TODO: refactor */
			if (integrator == VELOCITY_VERLET) {
				/* calc initial a */
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
			/* TODO: refactor */
			} else if (integrator == SEMI_IMPLICIT_EULER) {
				/*
				 * explicit euler
				 * wait this isn't explicit euler
				 * this is semi-implicit euler
				 */
				planets.dx[j] += dt * force_buffer[2*j];
				planets.dy[j] += dt * force_buffer[2*j+1];
				planets.x[j] += planets.dx[j] * dt;
				planets.y[j] += planets.dy[j] * dt;
			/* TODO: refactor */
			} else {
				/* this is explicit euler :( */
				planets.x[j] += planets.dx[j] * dt;
				planets.y[j] += planets.dy[j] * dt;
				planets.dx[j] += dt * force_buffer[2*j];
				planets.dy[j] += dt * force_buffer[2*j+1];
			}

			/* add kinetic energy */
			E += 0.5 * planets.M[j] * (planets.dx[j] * planets.dx[j]
						  +planets.dy[j] * planets.dy[j]);

			/* TODO: refactor */
			/* write output */
			if (output == BINFILE) {
				if (j == 0)
					write(file, &E, sizeof(E));

				write(file, &planets.x[j], sizeof(planets.x[j]));
				write(file, &planets.y[j], sizeof(planets.y[j]));

				f_t L_z = planets.M[j] * (planets.x[j] * planets.dy[j]
							 -planets.y[j] * planets.dx[j]);

				write(file, &L_z, sizeof(L_z));
			}
		}
		if (integrator == VELOCITY_VERLET)
			memcpy(old_force_buffer, force_buffer, sizeof(force_buffer));

		memset(force_buffer, 0, sizeof(force_buffer));

		if (output == RAYLIB && i % frameskip == 0) {

			BeginDrawing();

			ClearBackground(BLACK);

			DrawText(TextFormat("frameskip: %d", frameskip), 10, 30, 20, DARKGREEN);

			DrawFPS(10,10);

			Vector2 sunPos = {50*planets.x[0]+400, 50*planets.y[0]+300};
			DrawCircleV(sunPos, 20, GOLD);

			Vector2 earthPos = {50*planets.x[1]+400, 50*planets.y[1]+300};
			DrawCircleV(earthPos, 5, BLUE);

			Vector2 jupPos = {50*planets.x[2]+400, 50*planets.y[2]+300};
			DrawCircleV(jupPos, 9, BEIGE);

			EndDrawing();

		}
		i++;
		if (output == RAYLIB && WindowShouldClose())
			break;
		if (i * dt >= t)
			break;
	}

	free_planets(&planets);

	if (output == RAYLIB)
		CloseWindow();
	if (output == BINFILE)
		close(file);

	return EXIT_SUCCESS;
}
