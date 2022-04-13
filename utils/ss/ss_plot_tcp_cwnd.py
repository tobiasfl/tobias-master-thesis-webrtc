import matplotlib.pyplot as plt
import matplotlib.dates
import sys
import re
from datetime import datetime
from datetime import timedelta
import pandas as pd

#original line with cwnd
#ts,ip:port,,,cwnd,ssthresh,rtt
#1638953696.353329,10.0.0.2:38624,,,23,13

args = sys.argv


if len(args) != 3:
    print("Usage: <csv file> [cwnd|rtt|ssthresh|cwnd_bytes] ")
    sys.exit(1)

cc_vals = {"cwnd":4, "ssthresh":5, "rtt":6, "cwnd_bytes":4}
if args[2] not in cc_vals:
    print("invalid cc attribute")
    sys.exit(1)

def extract_info(list_of_lines, cc):
    result = []

    for line in list_of_lines:
        splitted = line.strip('\n').split(',')
        
        ts_split = splitted[0].split('.')
        if not ts_split[0].isdigit() or not ts_split[1].isdigit():
            continue

    
        dt = datetime.fromtimestamp(int(ts_split[0]))
        delta = timedelta(microseconds=int(ts_split[1]))

        val = float(splitted[cc_vals[cc]])

        line_res = (dt + delta, val)
        result.append(line_res)
    
    return result
    
def plot(list_of_tuples, cc):
    if len(list_of_tuples) == 0:
        return 

    #make first timestamp start from 0 and convert to int of ms
    timestamps = [(ts-list_of_tuples[0][0]).total_seconds()*1000 for (ts, v) in list_of_tuples]
    cc_vals = [v for (ts, v) in list_of_tuples] 

    plt.plot(timestamps, cc_vals, label = cc)
    
    plt.xlabel("time")
    plt.ylabel(cc)

    plt.xlim(left=0)
    plt.ylim(0)

    plt.legend()

    plt.savefig("ss-out-" + cc + ".png")
    plt.show()


with open(args[1]) as logfile:
    lines = logfile.readlines()

    data = []

    if args[2] == "cwnd_bytes":
        data = extract_info(lines, "cwnd")
        TCP_MSS = 1448
        data = [(ts, v*TCP_MSS) for (ts, v) in data]
    else:
        data = extract_info(lines, args[2])

    plot(data, args[2])
