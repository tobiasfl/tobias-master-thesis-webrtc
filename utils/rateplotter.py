#!/usr/bin/python3

import matplotlib.pyplot as plt
#import numpy as np
import sys

#Original lines to parse:
#[13318:12:0826/200633.769709:INFO:send_side_bandwidth_estimation.cc(650)] TOBIAS(before) FLOW-1 3253799 300

args = sys.argv

if len(args) != 3:
    print("Usage: rateplotter.py <logfile.txt> <output file name prefix")
    sys.exit(1)

data = []

with open(args[1]) as logfile:
    lines = logfile.readlines()
    for line in lines:
        if "PLOT_THIS" in line:
            splitted = line.split()
            splitlen = len(splitted)
            #(flow id, timestamp, rate)
            data.append((splitted[splitlen-3], int(splitted[splitlen-2]), int(splitted[splitlen-1])))


#write the tuples to file
with open(args[2] + ".txt", "w") as extractedlogfile:
    extractedlogfile.write("\n".join([" ".join(map(str, row)) for row in data]))

#make timestamps start from 0 (it varies at what point the communicaton/cc is started)
if len(data) > 0:
    data = [(objid, ts - data[0][1], rate) for (objid, ts, rate) in data]

id_grouped = {}
for objid, ts, rate in data:
    if objid in id_grouped:
        id_grouped[objid].append((objid, ts, rate))
    else:
        id_grouped[objid] = [(objid, ts, rate)]



for group in list(id_grouped.values()):
    #sort by timestamp
    group.sort(key=lambda tup: tup[1])
    objid = group[0][0]
    timestamps = [ts for (objid, ts, rate) in group]
    rates = [rate for (objid, ts, rate) in group]
    plt.plot(timestamps, rates, label = objid)

plt.xlabel("Time from first recorded target update (ms)")
plt.ylabel("Rate (kbps)")

plt.xlim(left=0)
plt.ylim(0)

plt.legend()

plt.savefig(args[2] + ".png")
plt.show()
