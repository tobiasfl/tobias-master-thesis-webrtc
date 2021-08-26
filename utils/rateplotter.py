#!/usr/bin/python3

import matplotlib.pyplot as plt
#import numpy as np
import sys

#Original lines to parse:
#[13318:12:0826/200633.769709:INFO:send_side_bandwidth_estimation.cc(650)] TOBIAS(before) 0x28d5137c1a20 3253799 300

args = sys.argv

if len(args) != 4:
    print("Usage: rateplotter.py <logfile.txt> <extracted-log-data-fname> <plotfile-fname>")
    sys.exit(1)

data = []

with open(args[1]) as logfile:
    lines = logfile.readlines()
    for line in lines:
        if "TOBIAS" in line:
            splitted = line.split()
            #(object-address, timestamp, rate)
            data.append((splitted[2], int(splitted[3]), int(splitted[4])))

#make timestamps start from 0 (it varies at what point the communicaton/cc is started)
if len(data) > 0:
    data = [(objaddr, ts - data[0][1], rate) for (objaddr, ts, rate) in data]

with open(args[2], "w") as extractedlogfile:
    extractedlogfile.write("\n".join([" ".join(map(str, row)) for row in data]))

objaddr_grouped = {}
for objaddr, ts, rate in data:
    if objaddr in objaddr_grouped:
        objaddr_grouped[objaddr].append((objaddr, ts, rate))
    else:
        objaddr_grouped[objaddr] = [(objaddr, ts, rate)]



for group in list(objaddr_grouped.values()):
    #sort by timestamp
    group.sort(key=lambda tup: tup[1])

    objaddr = group[0][0]
    timestamps = [ts for (objaddr, ts, rate) in group]
    rates = [rate for (objaddr, ts, rate) in group]
    plt.plot(timestamps, rates, label = objaddr)

plt.xlabel("Time from first recorded target update (ms)")
plt.ylabel("Rate (kbps)")

plt.xlim(0)
plt.ylim(0)

plt.savefig(args[3])
plt.show()
