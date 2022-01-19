#!/bin/bash

#TODO: add args for choosing web page/number and type of flows
if [ $# -lt 5 ]; then
    echo "USAGE: $0 <bandwidth mbit/s limit> <delay ms> <queue length pkts> <test run length sec.> <output directory>"
    exit 1
fi

out_dir=$5/$1mbit_$2ms_$3pkts_$4sec_chromium_test
#Create directory for output data
if [ -d $5 ]; then
    mkdir $out_dir
else
    echo "Error: Directory $5 does not exist."
    exit 1
fi

#ssh into router, run limitation script with arguments from this script
ssh -t tobias@10.0.0.1 "sudo bash ~/Code/tobias-master-thesis-webrtc/utils/tc/limit_router.sh $1 $2 $3"

#TODO: ssh into receiver and start chrome from terminal(and maybe exit after everything is finished)

#start tcpdump, store pid so it can be exited after chromium has finished its run
#TODO: may also start tcpdumps at router and receiver if neccessary
tcpdump -i enp0s31f6 -w $out_dir/if_dump.pcap &
tcpdump_pid=$!

#run chromium for some time before exiting
timeout $4 bash run_chromium.sh $out_dir
chromium_pid=$!

wait $chromium_pid
#stop dumping once, chromium run finished
kill $tcpdump_pid
