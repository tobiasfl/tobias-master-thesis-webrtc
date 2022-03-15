import matplotlib.pyplot as plt
import matplotlib.dates
import sys
import re
from datetime import datetime
import pandas as pd
from plotting import *

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

    df = pd.DataFrame(result, columns=["Time", "Value", "Id"])
    df = pd.pivot_table(df, index=["Time"], columns="Id", values="Value")

    earliest_date = df.index.min()

    #To make Time not be index anymore so it can be altered
    df.reset_index(inplace=True)
    df["Time"] = df["Time"] - earliest_date
 
    #To remove NaN's
    #TODO: this leads to a bug where all lines are kept until the end even though they have
    #for instance already left the FSE-NG
    for flow_col in df.columns[1:]:
        df[flow_col] = df[flow_col].interpolate(limit=len(df.columns[1:])-1)

    return df

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

    if args[1] == "cwnd_segments":
        df = extract_info(lines, 'cwnd', 'PLOT_THIS')

        for flow_col in df.columns[1:]:
            df[flow_col] = cwnd2cwnd_segments(df[flow_col]) 

        plot_scatter_plot(df, "Time (s)", args[1])

    elif args[1] == "max_cwnd_segments":
        df = extract_info(lines, 'max_cwnd', 'PLOT_THIS')

        for flow_col in df.columns[1:]:
            df[flow_col] = cwnd2cwnd_segments(df[flow_col])

        plot_scatter_plot(df, "Time (s)", args[1])

    else:
        df = extract_info(lines, args[1], 'PLOT_THIS')
        plot_scatter_plot(df, "Time (s)", args[1])
    
