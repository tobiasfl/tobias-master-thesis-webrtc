import matplotlib.pyplot as plt
import matplotlib.dates
import sys
import re
from datetime import datetime
from datetime import timedelta
import pandas as pd

#original line with cwnd
#ts,ip:port,,,cwnd,ssthresh
#1638953696.353329,10.0.0.2:38624,,,23,13

args = sys.argv


if len(args) != 2:
    print("Usage: ss_plot_cwnd.py [cwnd|rtt|ssthresh] ")
    sys.exit(1)

#TODO: make script parse rtt as well
cc_vals = {"cwnd":4, "ssthresh":5, "rtt":6,}
if args[1] not in cc_vals:
    print("invalid cc attribute")
    sys.exit(1)

def extract_info(list_of_lines, cc):
    result = []

    for line in list_of_lines:
        splitted = line.split(',')
        
        ts_split = splitted[0].split('.')
        dt = datetime.fromtimestamp(int(ts_split[0]))
        delta = timedelta(microseconds=int(ts_split[1]))

        val = int(splitted[cc_vals[cc]])

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


with open('sender-ss.csv') as logfile:
    lines = logfile.readlines()
    data = extract_info(lines, args[1])
    plot(data, args[1])
