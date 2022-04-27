import matplotlib.pyplot as plt
import matplotlib.dates
import sys
import re
from datetime import datetime
import pandas as pd

def normalize_time_column(df):
    #TODO: this leads to a bug where all lines are kept until the end even though they have
    #for instance already left the FSE-NG

    earliest_date = df.index.min()

    #To make Time not be index anymore so it can be altered
    df.reset_index(inplace=True)
    df["Time"] = df["Time"] - earliest_date

def remove_NaNs(df):
    for flow_col in df.columns[1:]:
        df[flow_col] = df[flow_col].interpolate(limit=len(df.columns[1:])-1)

def extract_info(list_of_lines, cc_attribute, id_line):
    result = []

    #gets the HHMMSS.MICROSECONDS timestamp
    timestamp_regex = r"^\[\d+:\d+:\d+/(\d+\.\d+):"
    #get the identity of the thing, discard _'s in between because plt.legend() does not show them then
    id_regex = rf".+ {id_line}_*(\w*)\b"
    #gets cc_attribute value
    cc_attr_regex = rf"\b{cc_attribute}=(\-?\d+)"
    regex = rf"{timestamp_regex}{id_regex}.+{cc_attr_regex}"

    date_time_format = "%H%M%S.%f"

    for line in list_of_lines:
        line_res = re.findall(regex, line)
        if len(line_res) == 1 and len(line_res[0]) == 3:
            datetime_obj = datetime.strptime(line_res[0][0], date_time_format)
            cc_val = int(line_res[0][2])
            id_str = line_res[0][1]
            result.append((datetime_obj.timestamp(), cc_val, id_str))


    df = pd.DataFrame(result, columns=["Time", "Value", "Id"])
    df = pd.pivot_table(df, index=["Time"], columns="Id", values="Value")

    return df

def cwnd2rate(rtt_ms, cwnd_bytes):
    rtt_us = rtt_ms/1000
    cwnd_bits = cwnd_bytes*8
    return 1000000*(cwnd_bits/rtt_us)

def cwnd2cwnd_segments(cwnd_bytes):
    SCTP_MTU = 1267 #based on tcpdump
    SCTP_MSS = 1191 #based on checking difference for each cwnd increase in log file
    return cwnd_bytes/SCTP_MTU


