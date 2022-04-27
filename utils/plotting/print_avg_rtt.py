import pandas as pd
from extract_log_data import *
from avg_throughput import *

#TODO: THIS IS INCORRECT
def round_rtt_data_to_1s(df):
        earliest_date =  df.index.min()

        df.reset_index(inplace=True)
        df["Time"] = df["Time"] - earliest_date
        df["Time"] = df["Time"].round(0)
        df.groupby(df["Time"]).mean()
        df = df.drop_duplicates(subset=["Time"])

        return df



def main():
    args = sys.argv
    if len(args) != 5:
        print(f'Usage: {args[0]} <root dir> <start time(s)> <end time(s)> <case-filter>')
        sys.exit(1)

    dir_name = args[1]
    start = int(args[2])
    end = int(args[3])
    case_filter = args[4]

    log_files = [f for f in get_log_files(dir_name) if case_filter in f]

    gcc_sum_avg_rtts = 0
    sctp_sum_avg_rtts = 0
    for log_file_name in log_files:
        print(log_file_name)
        with open(log_file_name) as logfile:
            lines = logfile.readlines()
             
            #TODO: must round it to something first or it will not make sense!!!
            rttsctp_df = extract_info(lines, 'rttsctp', 'PLOT_THIS')
            remove_NaNs(rttsctp_df)
            rttsctp_df = round_rtt_data_to_1s(rttsctp_df)
            sctp_avg_rtt = rttsctp_df[rttsctp_df.columns[0]].mean()
            print("SCTP Avg. RTT(ms): " + str(sctp_avg_rtt))

            rttgcc_df = extract_info(lines, 'rttgcc', 'PLOT_THIS')
            remove_NaNs(rttgcc_df)
            rttgcc_df = round_rtt_data_to_1s(rttgcc_df)
            gcc_avg_rtt = rttgcc_df[rttgcc_df.columns[0]].mean()
            print("GCC Avg. RTT(ms): " + str(gcc_avg_rtt))

            gcc_sum_avg_rtts = gcc_sum_avg_rtts + gcc_avg_rtt
            sctp_sum_avg_rtts = sctp_sum_avg_rtts + sctp_avg_rtt
    print('***ALL COMBINED')
    final_avg_gcc_rtt = round(gcc_sum_avg_rtts / len(log_files), 0)
    print('Final avg. GCC RTT' + str(final_avg_gcc_rtt))
    final_avg_sctp_rtt = round(sctp_sum_avg_rtts / len(log_files), 0)
    print('Final avg. SCTP RTT' + str(final_avg_sctp_rtt))

def get_log_files(dir_name):
    all_files = get_list_of_files(dir_name)
    return [lfile for lfile in all_files if '/log.txt' in lfile]


main()
