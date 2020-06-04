#! /usr/bin/env python3

import argparse
import numpy as np


def f(x):
    return np.sqrt(1-x**2)

parser = argparse.ArgumentParser("""Integrate y = sqrt(1-x^2) over x from 0 to 1
                                 using simple Monte Carlo integration""")
parser.add_argument("-N", type=int, default=1000, help="""The number of random points used.""")
parser.add_argument("-i", type=int, default=1, help="""The number of
                    iterations used.""")

args=parser.parse_args()

N = args.N

iterations = args.i

rg = np.random.Generator(np.random.PCG64())

fm = 0
fm_sqr = 0
accepted = 0

for i in range(iterations):
    x = rg.random(N, dtype='d')
    y = f(x)
    fm = np.mean(y)
    fm_sqr = np.mean((y)**2)
    accepted = np.sum(x <= y)

fm /= iterations
fm_sqr /= iterations

simple_res = fm
simple_err = np.sqrt((fm_sqr - fm**2)/N)

rejection_res = accepted / (N * iterations)
print("Points used: {0} Iterations: {1}".format(N, iterations))
print("Analytical solution is: π/4 ≈ 0.78540")
print("Simple MC solution: {0:.5f} ±  {1:.5f}".format(simple_res, simple_err))
print("Rejection MC Solution: {0:.5f}".format(rejection_res))
