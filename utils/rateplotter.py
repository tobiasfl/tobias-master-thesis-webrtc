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
            splitlen = len(splitted)
            #(object-address, timestamp, rate)
            data.append((splitted[splitlen-3], int(splitted[splitlen-2]), int(splitted[splitlen-1])))

#make timestamps start from 0 (it varies at what point the communicaton/cc is started)
if len(data) > 0:
    data = [(objid, ts - data[0][1], rate) for (objid, ts, rate) in data]

with open(args[2], "w") as extractedlogfile:
    extractedlogfile.write("\n".join([" ".join(map(str, row)) for row in data]))

objaddr_grouped = {}
for objid, ts, rate in data:
    if objid in objaddr_grouped:
        objaddr_grouped[objid].append((objid, ts, rate))
    else:
        objaddr_grouped[objid] = [(objid, ts, rate)]



for group in list(objaddr_grouped.values()):
    #sort by timestamp
    group.sort(key=lambda tup: tup[1])
    objid = group[0][0]
    timestamps = [ts for (objid, ts, rate) in group]
    rates = [rate for (objid, ts, rate) in group]
    plt.plot(timestamps, rates, label = objid)

plt.xlabel("Time from first recorded target update (ms)")
plt.ylabel("Rate (kbps)")

plt.xlim(left=0, right=15000)
plt.ylim(0)

plt.legend()

plt.savefig(args[3])
plt.show()
