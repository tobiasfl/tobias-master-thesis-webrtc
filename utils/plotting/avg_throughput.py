import subprocess
import sys
import os

def find_duration_s(list_of_lines):
    duration_line = next(x for x in list_of_lines if "Duration:" in x) 
    splitted = duration_line.split()
    return float(splitted[2])

def find_total_bytes(list_of_lines):
    #We know it is in third to last line
    total_bytes_line = list_of_lines[len(list_of_lines)-3]
    splitted = total_bytes_line.split()
    return int(splitted[7])

def calculate_avg_throghput_mbps(duration_secs, total_bytes):
    bps = (total_bytes*8) / duration_secs
    return round(bps/1000000, 3)

def run_tshark(rtp_filter, pcap_filename):
    return subprocess.run(['tshark','-r',pcap_filename,'-R',rtp_filter,'-2','-z','io,stat,0'], capture_output=True, text=True).stdout

def get_list_of_files(dir_name):
    list_of_files = os.listdir(dir_name)
    all_files = []

    for entry in list_of_files:
        full_path = os.path.join(dir_name, entry)
        if os.path.isdir(full_path):
            all_files = all_files + get_list_of_files(full_path)
        else:
            all_files.append(full_path)
    return all_files 

def get_pcap_files(dir_name):
    all_files = get_list_of_files(dir_name)
    return [pcap_file for pcap_file in all_files if pcap_file.endswith('.pcap')]


def check_args_and_get_dir():
    args = sys.argv
    if len(args) != 2:
        print(f'Usage: {args[0]} <root dir>')
        sys.exit(1)
    return args[1]

def main():
    dir_name = check_args_and_get_dir()
    pcap_files = get_pcap_files(dir_name)

    sum_avg_tputs = 0
    for pcap_file in pcap_files:
        print(pcap_file)
        splitted_result = run_tshark('(ip.dst==192.168.0.2 && rtp)', pcap_file).split('\n')

        total_bytes = find_total_bytes(splitted_result)

        if total_bytes == 0:
            splitted_result = run_tshark('(ip.dst==192.168.0.2 && udp && !(dtls) && !rtp)', pcap_file).split('\n')
            total_bytes = find_total_bytes(splitted_result)

        duration_s = find_duration_s(splitted_result)

        avg_tput = calculate_avg_throghput_mbps(duration_s, total_bytes)
        sum_avg_tputs = sum_avg_tputs + avg_tput

        print('Duration(s): ' + str(duration_s))
        print('Bytes: ' + str(total_bytes))
        print('Avg. tput(Mbps): ' + str(avg_tput))

    print('Final avg. tput(Mbps): ' + str(sum_avg_tputs / len(pcap_files)))

main()
