import matplotlib.pyplot as plt
import matplotlib.dates
import sys
import re
from datetime import datetime
import pandas as pd

#original line with cwnd
#[12452:15:1127/133332.035937:VERBOSE1:retransmission_queue.cc(463)] DcSctpTransport0: tx: Sending TSN 193001767 - 40 bytes. outstanding_bytes=40 (0), cwnd=11910, rwnd=131032 (131072)

args = sys.argv

valid_cc_vals = {"cwnd", "ssthresh", "rtt", "srtt", "rto", "rate", "cwnd_segments"}

if len(args) != 4:
    print("Usage: plot_cwnd.py <logfile.txt> <outfile name> [cwnd|rtt|srtt|ssthresh|cwnd_segments] ")
    sys.exit(1)

if args[3] not in valid_cc_vals:
    print("invalid cc attribute")
    sys.exit(1)



def extract_info(list_of_lines, cc_attribute, filename):
    result = []

    #gets the HHMMSS.MICROSECONDS timestamp
    timestamp_regex = r"^\[\d+:\d+:\d+/(\d+\.\d+):"
    #get the identity of the SctpTransport
    sctp_id_regex = r".+ (DcSctpTransport\d+)"
    #gets cc_attribute value
    cc_attr_regex = rf".+ {cc_attribute}=(\d+)"
    regex = rf"{timestamp_regex}{sctp_id_regex}{cc_attr_regex}"

    date_time_format = "%H%M%S.%f"

    lines_to_save = []
    for line in list_of_lines:
        line_res = re.findall(regex, line)
        if len(line_res) == 1 and len(line_res[0]) == 3:
            datetime_obj = datetime.strptime(line_res[0][0], date_time_format)
            cc_val = int(line_res[0][2])
            sctp_id = line_res[0][1]
            #multiply by 1000 to convert posix timestamp to ms
            result.append((datetime_obj.timestamp()*1000, cc_val, sctp_id))
            lines_to_save.append(line)

    with open(filename + ".txt", "w") as extractedlogfile:
        print("creating log file with name:" + filename + ".txt")
        extractedlogfile.write("".join(lines_to_save))

    return result

def rate_aggregate(cwnd_tuples, rtt_tuples):
    if len(cwnd_tuples) == 0 or len(rtt_tuples) == 0:
        return []

    cwnd_df = pd.DataFrame(cwnd_tuples, columns=['timestamp', 'cwnd', 'sctp_id']) 
    rtt_df = pd.DataFrame(rtt_tuples, columns=['timestamp', 'rtt', 'sctp_id']) 

    merged_df = cwnd_df.merge(rtt_df, on=['timestamp', 'sctp_id'], how='outer')
    #TODO: this is not finished


def plot(list_of_tuples, cc_attribute, filename):
    if len(list_of_tuples) == 0:
        return 

    id_grouped = {}
    for tup in list_of_tuples:
        if tup[2] in id_grouped:
            id_grouped[tup[2]].append(tup)
        else:
            id_grouped[tup[2]] = [tup]

    for group in list(id_grouped.values()):
        timestamps = []
        for (d, v, sctp_id) in group:
            #make first timestamp start from 0
            timestamps.append((d-group[0][0]))
        cc_vals = [v for (d, v, sctp_id) in group] 

        plt.plot(timestamps, cc_vals, label = group[0][2])
    
    plt.xlabel("time")
    plt.ylabel(cc_attribute)

    plt.xlim(left=0)
    plt.ylim(0)

    plt.legend()

    plt.savefig(filename + ".png")
    plt.show()


with open(args[1]) as logfile:
    lines = logfile.readlines()

    data = []

    if args[3] == 'rate':
        cwnd_data = extract_info(lines, 'cwnd', '/tmp/cwnd_log_data')
        rtt_data = extract_info(lines, 'rtt', '/tmp/rtt_log_data')
        rate_aggregate(cwnd_data, rtt_data)        
    elif args[3] == "cwnd_segments":
        data = extract_info(lines, 'cwnd', args[2])
        SCTP_MSS = 1230
        data = [(ts, cc_val/SCTP_MSS, sctp_id) for (ts, cc_val, sctp_id) in data]
    else:
        data = extract_info(lines, args[3], args[2])
    
    plot(data, args[3], args[2])
