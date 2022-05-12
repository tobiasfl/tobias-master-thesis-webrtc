import pandas as pd
from extract_log_data import *
from avg_throughput import *
from plotting import *

def main():
    args = sys.argv
    if len(args) != 5:
        print(f'Usage: {args[0]} <root dir> <start time(s)> <end time(s)> <mechanism filter>')
        sys.exit(1)

    dir_name = args[1]
    start = int(args[2])
    end = int(args[3])
    arg_filter = args[4]

    fn_filter = 'recv_if_dump'

    pcap_files = [f for f in get_pcap_files(dir_name) if fn_filter in f and arg_filter in f]
    
    gcc_avg_tputs_list = []
    sctp_avg_tputs_list = []
    for prio_case in ['prio_case'+str(x)+'/' for x in range(1, 11)]:
        curr_pcaps = [f for f in pcap_files if prio_case in f]
        curr_gcc_tputs = []
        curr_sctp_tputs = []
        for pcap in curr_pcaps:
            print(pcap)
            curr_gcc_tputs = curr_gcc_tputs + gcc_avg_tputs(pcap, start, end)
            curr_sctp_tputs = curr_sctp_tputs + [sctp_avg_tputs(pcap, start, end)]
        print(curr_gcc_tputs)
        gcc_avg = sum(curr_gcc_tputs) / len(curr_gcc_tputs)
        sctp_avg = sum(curr_sctp_tputs) / len(curr_sctp_tputs)
        gcc_avg_tputs_list = gcc_avg_tputs_list + [gcc_avg]
        sctp_avg_tputs_list = sctp_avg_tputs_list + [sctp_avg]
    
    print(gcc_avg_tputs_list)
    print(sctp_avg_tputs_list)
    plot_bw_differing_prios_10x10(gcc_avg_tputs_list, sctp_avg_tputs_list)


main()
