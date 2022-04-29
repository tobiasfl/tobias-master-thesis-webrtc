import pandas as pd
from avg_throughput import *
from plotting import *

def get_all_dfs(dir_name):
    fn_filter = 'recv_if_dump'

    pcap_files = [f for f in get_pcap_files(dir_name) if fn_filter in f] 
    return [get_df(pcap) for pcap in pcap_files]
#TODO: scrip not done
def plot_jfi_over_time():
    args = sys.argv
    if len(args) != 4:
        print(f'Usage: {args[0]} <root dir> <start time(s)> <end time(s)>')
        sys.exit(1)

    dir_name = args[1]
    start = int(args[2])
    end = int(args[3])

    dfs = get_all_dfs(dir_name)
    print(dfs[0])
    
plot_jfi_over_time()
