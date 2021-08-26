#!/usr/bin/python3

import matplotlib.pyplot as plt
#import numpy as np
import sys

args = sys.argv

if len(args) != 2:
    print("Usage: rateplotter.py <input-file-name.txt>")
    sys.exit(1)

with open(args[1]) as inputfile:
    lines = inputfile.readlines()
    timestamps = [line.split()[0] for line in lines]
    rates = [line.split()[1] for line in lines]

fig, ax = plt.subplots()
ax.plot(timestamps, rates)

plt.savefig("test1")
plt.show()
