from avg_throughput import *

def main():
    args = sys.argv
    if len(args) != 5:
        print(f'Usage: {args[0]} <root dir> <start time(s)> <end time(s)> <case-filter>')
        sys.exit(1)

    dir_name = args[1]
    start = int(args[2])
    end = int(args[3])
    case_filter = args[4]
    #TODO: could do dynamically instead
    fn_filter = 'recv_if_dump'

    pcap_files = [f for f in get_pcap_files(dir_name) if case_filter in f and fn_filter in f] 

    final_gcc_avg_tputs = []
    sctp_sum_avg_tputs = 0
    sctp_flow_count = 0
    for pcap_file in pcap_files:
        print(pcap_file)
        gcc_avg_tputs_list = gcc_avg_tputs(pcap_file, start, end)
        print(gcc_avg_tputs_list)

        total_tput = sum(gcc_avg_tputs_list)

        sctp_avg_tput = sctp_avg_tputs(pcap_file, start, end)

        print('***COMPARISON')
        if sctp_avg_tput != 0:
            total_tput = total_tput + sctp_avg_tput
            sctp_flow_count = sctp_flow_count + 1
            sctp_sum_avg_tputs = sctp_sum_avg_tputs + sctp_avg_tput

        print('Total avg. tput(Mbps): ' + str(total_tput))
        for gcc_avg_tput in gcc_avg_tputs_list:
            gcc_share = gcc_avg_tput / total_tput
            print('GCC share of total tput: ' + str(gcc_share))
            final_gcc_avg_tputs.append(gcc_avg_tput)

        if sctp_avg_tput != 0:
            sctp_share = sctp_avg_tput / total_tput
            print('SCTP share of total tput: ' + str(sctp_share))

        print('Jains fairness index:' + str(jains_fairness(gcc_avg_tputs_list + [sctp_avg_tput])))

   
    print('***ALL COMBINED')
    final_avg_gcc_tput = round(sum(final_gcc_avg_tputs) / len(final_gcc_avg_tputs), 3)
    final_avg_sctp_tput = round(sctp_sum_avg_tputs  / sctp_flow_count, 3)
    final_avg_total_tput = round((sum(final_gcc_avg_tputs) / len(pcap_files)) + final_avg_sctp_tput, 3)
    final_gcc_share = round(final_avg_gcc_tput / final_avg_total_tput, 3)
    final_sctp_share = round(final_avg_sctp_tput / final_avg_total_tput, 3)

    print('Final avg. GCC tput(Mbps): ' + str(final_avg_gcc_tput))
    print('Final avg. SCTP tput(Mbps): ' + str(final_avg_sctp_tput))
    print('Final total avg. tput(Mbps): ' + str(final_avg_total_tput))

    if len(final_gcc_avg_tputs) == len(pcap_files):
        final_jfi = round(jains_fairness([final_avg_gcc_tput, final_avg_sctp_tput]), 3)
        print('Final jains_fairness index:' + str(final_jfi))
    print('Final GCC share of total tput(gcc flows combined):' + str(final_gcc_share))
    print('Final SCTP share of total tput(gcc flows combined):' + str(final_sctp_share))



main()
