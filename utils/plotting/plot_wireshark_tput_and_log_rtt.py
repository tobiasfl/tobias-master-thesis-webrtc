import pandas as pd
from plotting import *
from extract_log_data import *

def rename_df(df, rttsctp_df, rttgcc_df):
    renamings = {rttsctp_df.columns[1]:'SCTP'}
    for (key, num) in zip(rttgcc_df.columns[1:], range(1, len(rttgcc_df.columns))):
        renamings[key] = f'RTP {num}'

    return df.rename(columns=renamings)


def plot_wireshark_tput_and_log_rtt():
    with open("log.txt") as logfile:
        lines = logfile.readlines()

        rttsctp_df = extract_info(lines, 'rttsctp', 'PLOT_THIS')
        rttgcc_df = extract_info(lines, 'rttgcc', 'PLOT_THIS')
        earliest_date = min(rttsctp_df.index.min(), rttgcc_df.index.min())

        rttsctp_df.reset_index(inplace=True)
        rttsctp_df["Time"] = rttsctp_df["Time"] - earliest_date
        rttsctp_df["Time"] = rttsctp_df["Time"].round(1)
        rttsctp_df.groupby(rttsctp_df["Time"]).mean()
        rttsctp_df = rttsctp_df.drop_duplicates(subset=["Time"])

        
        rttgcc_df.reset_index(inplace=True)
        rttgcc_df["Time"] = rttgcc_df["Time"] - earliest_date
        rttgcc_df["Time"] = rttgcc_df["Time"].round(1)
        rttgcc_df.groupby(rttgcc_df["Time"]).mean()
        rttgcc_df = rttgcc_df.drop_duplicates()
  
        rtt_df = rttgcc_df.merge(rttsctp_df, how='outer', on="Time")
    
        rtt_df = rename_df(rtt_df, rttsctp_df, rttgcc_df)

        print(rttsctp_df)
        print(rttgcc_df)
        print(rtt_df)

        ws_df = get_df('recv_if_dump.csv')
        
        plot_tput_and_rtt_comparison(ws_df, rtt_df)
 

plot_wireshark_tput_and_log_rtt()
