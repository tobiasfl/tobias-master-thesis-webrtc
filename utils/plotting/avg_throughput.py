import subprocess
import numpy as np
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

def rtp_packets_find_ssrcs(list_of_files):
    relevant_rtp_lines = [line for line in list_of_files if 'Type-125' in line]
    ssrcs = set()
    for line in relevant_rtp_lines:
        for word in line.split():
            if 'SSRC' in word:
                ssrc = word[5:-1]
                ssrcs.add(ssrc) 
    return ssrcs 

def udp_packets_find_dst_ports(list_of_files):
    relevant_udp_lines = [line for line in list_of_files if 'UDP' in line]
    udp_ports = set()
    for line in relevant_udp_lines:
        splitted = line.split()
        udp_ports.add(splitted[9])

    return udp_ports

def calculate_avg_throghput_mbps(duration_secs, total_bytes):
    bps = (total_bytes*8) / duration_secs
    return round(bps/1000000, 3)

def jains_fairness(tputs):
    return np.sum(tputs) ** 2 / (len(tputs) * np.sum(np.square(tputs)))

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

def sctp_avg_tputs(pcap_file, start_time, end_time):
    print("***SCTP***") 
    wireshark_time_filter = create_wireshark_time_filter(start_time, end_time)
    wireshark_filter = f'(ip.dst==192.168.0.2 && dtls && dtls.app_data) && {wireshark_time_filter}'

    splitted_result = run_tshark(wireshark_filter, pcap_file).split('\n')

    total_bytes = find_total_bytes(splitted_result)

    duration_s = min(find_duration_s(splitted_result), end_time - start_time)

    avg_tput =  calculate_avg_throghput_mbps(duration_s, total_bytes)
    print('Duration(s): ' + str(duration_s))
    print('Bytes: ' + str(total_bytes))
    print('Avg. tput(Mbps): ' + str(avg_tput))

    return avg_tput

def create_wireshark_time_filter(start_time, end_time):
    return f'frame.time_relative > {start_time} && frame.time_relative < {end_time}'

def analyze_rtp_type_packets(pcap_file, start_time, end_time):
    wireshark_time_filter = create_wireshark_time_filter(start_time, end_time)
    rtp_wireshark_filter = f'(ip.dst==192.168.0.2 && rtp && {wireshark_time_filter})'

    splitted_result = run_tshark(rtp_wireshark_filter, pcap_file).split('\n')

    total_bytes = find_total_bytes(splitted_result)
    if total_bytes == 0:
        return []

    avg_tputs = []

    ssrc_set = rtp_packets_find_ssrcs(splitted_result)
    for ssrc in ssrc_set:
        print(f"***GCC SSRC={ssrc}***")
        wireshark_filter = f'{rtp_wireshark_filter} && rtp.ssrc=={ssrc}'
        splitted_result = run_tshark(wireshark_filter, pcap_file).split('\n')
        total_bytes = find_total_bytes(splitted_result)

        duration_s = min(find_duration_s(splitted_result), end_time - start_time)
        
        avg_tput =  calculate_avg_throghput_mbps(duration_s, total_bytes)
        print('Duration(s): ' + str(duration_s))
        print('Bytes: ' + str(total_bytes))
        print('Avg. tput(Mbps): ' + str(avg_tput))

        avg_tputs.append(avg_tput)

    return avg_tputs

def analyze_udp_type_packets(pcap_file, start_time, end_time):
    wireshark_time_filter = create_wireshark_time_filter(start_time, end_time)
    udp_wireshark_filter = f'(ip.dst==192.168.0.2 && udp && !(dtls) && !rtp) && {wireshark_time_filter}'
    splitted_result = run_tshark(udp_wireshark_filter, pcap_file).split('\n')
    total_bytes = find_total_bytes(splitted_result)
    if total_bytes == 0:
        return []

    avg_tputs = []

    udp_dst_port_set = udp_packets_find_dst_ports(splitted_result)
    for port in udp_dst_port_set:
        print(f"***GCC DST_UDP={port}***")
        wireshark_filter = f'{udp_wireshark_filter} && udp.dstport=={port}'
        splitted_result = run_tshark(wireshark_filter, pcap_file).split('\n')
        total_bytes = find_total_bytes(splitted_result)

        duration_s = min(find_duration_s(splitted_result), end_time - start_time)

        avg_tput =  calculate_avg_throghput_mbps(duration_s, total_bytes)
        print('Duration(s): ' + str(duration_s))
        print('Bytes: ' + str(total_bytes))
        print('Avg. tput(Mbps): ' + str(avg_tput))

        avg_tputs.append(avg_tput)

    return avg_tputs


def gcc_avg_tputs(pcap_file, start_time, end_time):
    avg_tputs = analyze_rtp_type_packets(pcap_file, start_time, end_time)

    avg_tputs = avg_tputs + analyze_udp_type_packets(pcap_file, start_time, end_time)

    return avg_tputs

