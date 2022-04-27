import matplotlib.pyplot as plt
import matplotlib.dates
import sys
import re
from datetime import datetime
import pandas as pd
from extract_log_data import *
from plotting import *

#original line with cwnd
#[12452:15:1127/133332.035937:VERBOSE1:retransmission_queue.cc(463)] DcSctpTransport0: tx: Sending TSN 193001767 - 40 bytes. outstanding_bytes=40 (0), cwnd=11910, rwnd=131032 (131072)
#[12452:15:1127/133332.035937:VERBOSE1:retransmission_queue.cc(463)] PLOT_THIS0: tx: Sending TSN 193001767 - 40 bytes. outstanding_bytes=40 (0), cwnd=11910, rwnd=131032 (131072)
#[71592:15:0125/125013.814224:INFO:usrsctp_transport.cc(132)] SCTP: PLOT_THIS_SCTP_CC51848878336640 cwnd=3804, rtt=0, mtu=1280, max_cwnd=0 rate=0

args = sys.argv

if len(args) != 2:
    print(f'Usage: {args[0]} <cc_val>')
    sys.exit(1)

with open("log.txt") as logfile:
    lines = logfile.readlines()

    line_finder = 'PLOT_THIS'

    dcsctp_stuff = ['ssthresh', 'cwnd', 'rtt', 'SScwnd', 'CAcwnd', 'packet_loss'] 
    if any(x == args[1] for x in dcsctp_stuff):
        line_finder = 'DcSctpTransport'

    if args[1] == "cwnd_segments":
        df = extract_info(lines, 'cwnd', line_finder)
        normalize_time_column(df)
        remove_NaNs(df)

        for flow_col in df.columns[1:]:
            df[flow_col] = cwnd2cwnd_segments(df[flow_col]) 

        plot_debug_line_plot(df, "Time (s)", args[1])

    elif args[1] == "max_cwnd_segments":
        df = extract_info(lines, 'max_cwnd', line_finder)
        normalize_time_column(df)
        remove_NaNs(df)

        for flow_col in df.columns[1:]:
            df[flow_col] = cwnd2cwnd_segments(df[flow_col])

        plot_debug_line_plot(df, "Time (s)", args[1])

    else:
        df = extract_info(lines, args[1], line_finder)
        normalize_time_column(df)
        remove_NaNs(df)
        #plot_debug_line_plot(df, "Time (s)", args[1])
        plot_scatter_plot(df, "Time (s)", args[1])
    
