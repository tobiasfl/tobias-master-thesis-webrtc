#!/bin/bash

#TODO: add args for choosing web page/number and type of flows
if [ $# -lt 5 ]; then
    echo "USAGE: $0 <bandwidth mbit/s limit> <delay ms> <queue length pkts> <test run length sec.> <output directory>"
    exit 1
fi

#Create directory for output data
if [ -d $5 ]; then
    mkdir $5/$1mbit_$2ms_$3pkts_$4sec_chromium_test
else
    echo "Error: Directory $5 does not exist."
    exit 1
fi

#ssh into router, run limitation script with arguments from this script
ssh -t tobias@10.0.0.1 "sudo ~/Code/tobias-master-thesis-webrtc/utils/tc/limit_router.sh $1 $2 $3"

#start tcpdump: tcpdump -i enp0s31f6 -w ./sctp.pcap

#run chromium for some time before exiting
