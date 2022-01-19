import argparse
import os

parser = argparse.ArgumentParser(description='Run a chromium test, the webRTC application server must be running and interfaces must be configured.')
parser.add_argument('bw_mbit', metavar='BANDWIDTH', type=int, help='Bandwidth limit in mbit/s')
parser.add_argument('delay_ms',  metavar='DELAY', type=int,  help='Extra delay in ms')
parser.add_argument('queue_pkts', metavar='QUEUELEN', type=int,  help='Size of buffer in packets')
parser.add_argument('test_len', metavar='TEST_LEN', type=int,  help='How many seconds for the test to run')
parser.add_argument('out_dir', metavar='OUT_DIR', type=str,  help='Where to place the directory with data')

def create_test_dir(bw, delay, buf, test_len, out_dir):
    if os.path.isdir(out_dir):
        new_dir_path = os.path.join(out_dir, f'{bw}mbit_{delay}ms_{buf}bufpkts_{test_len}sec_chromium_test')
        print('making output directory', new_dir_path)
        os.mkdir(new_dir_path)
    else:
        print('Invalid output directory')
        exit(1)

def run(bw, delay, buf, test_len, out_dir):
    print('running')
    #start tcpdump: tcpdump -i enp0s31f6 -w ./sctp.pcap
    #ssh into router, run limitation script with arguments from this script
    


args = parser.parse_args()
print(args)

create_test_dir(args.bw_mbit, args.delay_ms, args.queue_pkts, args.test_len, args.out_dir)
