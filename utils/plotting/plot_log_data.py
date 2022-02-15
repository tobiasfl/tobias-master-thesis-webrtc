import matplotlib.pyplot as plt
import matplotlib.dates
import sys
import re
from datetime import datetime
import pandas as pd

#original line with cwnd
#[12452:15:1127/133332.035937:VERBOSE1:retransmission_queue.cc(463)] DcSctpTransport0: tx: Sending TSN 193001767 - 40 bytes. outstanding_bytes=40 (0), cwnd=11910, rwnd=131032 (131072)
#[12452:15:1127/133332.035937:VERBOSE1:retransmission_queue.cc(463)] PLOT_THIS0: tx: Sending TSN 193001767 - 40 bytes. outstanding_bytes=40 (0), cwnd=11910, rwnd=131032 (131072)
#[71592:15:0125/125013.814224:INFO:usrsctp_transport.cc(132)] SCTP: PLOT_THIS_SCTP_CC51848878336640 cwnd=3804, rtt=0, mtu=1280, max_cwnd=0 rate=0

args = sys.argv

if len(args) != 2:
    print(f'Usage: {args[0]} <cc_val>')
    sys.exit(1)

def extract_info(list_of_lines, cc_attribute, id_string):
    result = []

    #gets the HHMMSS.MICROSECONDS timestamp
    timestamp_regex = r"^\[\d+:\d+:\d+/(\d+\.\d+):"
    #get the identity of the thing, discard _'s in between because plt.legend() does not show them then
    id_regex = rf".+ {id_string}_*(\w*)\b"
    #gets cc_attribute value
    cc_attr_regex = rf"\b{cc_attribute}=(\-?\d+)"
    regex = rf"{timestamp_regex}{id_regex}.+{cc_attr_regex}"

    date_time_format = "%H%M%S.%f"

    lines_to_save = []
    for line in list_of_lines:
        line_res = re.findall(regex, line)
        if len(line_res) == 1 and len(line_res[0]) == 3:
            datetime_obj = datetime.strptime(line_res[0][0], date_time_format)
            cc_val = int(line_res[0][2])
            id_str = line_res[0][1]
            result.append((datetime_obj.timestamp(), cc_val, id_str))
            lines_to_save.append(line)

    with open(cc_attribute + "_from_log" + ".txt", "w") as extractedlogfile:
        extractedlogfile.write("".join(lines_to_save))

    return result

def plot(list_of_tuples, cc_attribute, filename):
    if len(list_of_tuples) == 0:
        return 

    id_grouped = {}
    for tup in list_of_tuples:
        if tup[2] in id_grouped:
            id_grouped[tup[2]].append(tup)
        else:
            id_grouped[tup[2]] = [tup]

    #find earliest timestamp so that we can adjust that the earliest timestamp starts at 0
    earliest_timestamp = min([g[0][0] for g in list(id_grouped.values())])

    for group in list(id_grouped.values()):
        timestamps = []
        for (d, v, sctp_id) in group:
            #make first timestamp start from 0
            timestamps.append((d-earliest_timestamp))
        cc_vals = [v for (d, v, sctp_id) in group]  

        #TODO: find a way to give the lines that become hidden, a bit higher width
        plt.plot(timestamps, cc_vals, label=group[0][2], linewidth=2)
    

    #TODO: Have not checked the effects of all these different settings
    params = {
              'backend': 'ps',
              'axes.labelsize': 18,
              'font.weight' : 'bold',
              'legend.fontsize': 14,
              'xtick.labelsize': 15,
              'ytick.labelsize': 15,
              'savefig.dpi': 400,
              'axes.labelweight' : 'bold',
              }
    plt.rcParams.update(params)


    plt.xlabel("time(s)")
    plt.ylabel(cc_attribute)

    plt.xlim(left=0)
    plt.ylim(0)

    plt.grid(which="both")

    plt.tight_layout()

    plt.legend(loc="lower right")

    plt.savefig(filename + ".png")
    plt.show()

def cwnd2rate(rtt_ms, cwnd_bytes):
    rtt_us = rtt_ms/1000
    cwnd_bits = cwnd_bytes*8
    return 1000000*(cwnd_bits/rtt_us)

def cwnd2cwnd_segments(cwnd_bytes):
    SCTP_MTU = 1267 #based on tcpdump
    SCTP_MSS = 1191 #based on checking difference for each cwnd increase in log file
    return cwnd_bytes/SCTP_MTU


with open("log.txt") as logfile:
    lines = logfile.readlines()

    data = []

    if args[1] == "cwnd_segments":
        data = extract_info(lines, 'cwnd', 'PLOT_THIS')
        data = [(ts, cwnd2cwnd_segments(cc_val), sctp_id) for (ts, cc_val, sctp_id) in data]
    elif args[1] == "max_cwnd_segments":
        data = extract_info(lines, 'max_cwnd', 'PLOT_THIS')
        data = [(ts, cwnd2cwnd_segments(cc_val), sctp_id) for (ts, cc_val, sctp_id) in data]
    else:
        data = extract_info(lines, args[1], 'PLOT_THIS')
    
    plot(data, args[1], args[1])
