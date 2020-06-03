#! /usr/bin/env python3

import argparse
import numpy as np


def f(x):
    return np.sqrt(1-x**2)

parser = argparse.ArgumentParser("""Integrate y = sqrt(1-x^2) over x from 0 to 1
                                 using simple Monte Carlo integration""")
parser.add_argument("--N", type=int, default=1000, help="""The number of random points used.""")

args=parser.parse_args()

N = args.N

rg = np.random.Generator(np.random.PCG64())

x = rg.random(N, dtype='d')

fm = np.mean(f(x))
fm_sqr = np.mean((f(x))**2)

print("Analytical solution is: π/4 ≈ 0.78540")
print("MC solution with N: {0} is {1:.5f} ±  {2:.5f}".format(N, fm,
                                                              np.sqrt((fm_sqr -
                                                                       fm**2)/N)))
