import pandas as pd
from extract_log_data import *
from avg_throughput import *
from plotting import *
from functools import reduce

def sanitize(df, start, end):
    earliest_date =  df.index.min()
    df.reset_index(inplace=True)
    df["Time"] = df["Time"] - earliest_date
    df["Time"] = df["Time"].round(1)
    df.groupby(df["Time"]).mean()
    df = df.drop_duplicates(subset=["Time"])

    return df[df["Time"] >= start][df["Time"] <= end]


def main():
    args = sys.argv
    if len(args) != 5:
        print(f'Usage: {args[0]} <root dir> <start time(s)> <end time(s)> <case_filter>')
        sys.exit(1)

    dir_name = args[1]
    start = int(args[2])
    end = int(args[3])
    arg_case_filter = args[4]
   
    log_files = []
    for case_filter in ['none', 'FseNg/', 'FseNgV2/', 'FseV2/']:
        log_files = log_files + [f for f in get_log_files(dir_name) if case_filter in f and arg_case_filter in f]

    dfs = []
    for log_file_name in log_files:
        print(log_file_name)
        with open(log_file_name) as logfile:
            lines = logfile.readlines()

            rttgcc_df = extract_info(lines, 'rttsctp', 'PLOT_THIS')
            rttgcc_df = sanitize(rttgcc_df, start, end)
            dfs.append(rttgcc_df) 

    merged_df = reduce(lambda l,r: pd.merge(l,r,how='inner',on='Time'), dfs)

    plot_box_plot(merged_df)


def get_log_files(dir_name):
    all_files = get_list_of_files(dir_name)
    return [lfile for lfile in all_files if '/log.txt' in lfile]


main()
