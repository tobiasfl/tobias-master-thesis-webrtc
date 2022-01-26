#!/bin/bash

#TODO: add args for choosing web page/number and type of flows
if [ $# -lt 6 ]; then
    echo "USAGE: $0 <bandwidth mbit/s limit> <delay ms> <queue length pkts> <test run length sec.> <output directory> <web page>"
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
#ssh -t tobias@192.168.0.2 "google-chrome $6"

#TODO: may also start tcpdumps at router and receiver if neccessary

timeout $4 tcpdump -i enp0s31f6 -w $out_dir/if_dump.pcap &

#run chromium and tcpdump for some time before exiting
timeout $4 bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_chromium.sh $out_dir $6

