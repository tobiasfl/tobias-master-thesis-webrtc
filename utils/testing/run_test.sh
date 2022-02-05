#!/bin/bash

ROUTER=10.0.0.1
RECEIVER=192.168.0.2
SENDER=10.0.0.2

if [ $# -lt 6 ]; then
    echo "USAGE: $0 <bandwidth mbit/s> <delay ms> <queue length pkts> <test run length sec.> <output directory> <web page query config> [optional experiment label]"
    exit 1
fi

out_dir=$5/$1mbit_$2ms_$3pkts_$4sec_$6
if [ $# -eq 7 ]; then
    out_dir=$5/$7_$1mbit_$2ms_$3pkts_$4sec_$6  #if optional label is included
fi

#Create directory for output data
if [ -d $5 ]; then
    if [ -d $out_dir ]; then
        echo "deleting existing directory with same name"
        rm -rf $out_dir
    fi
    mkdir $out_dir
else
    echo "Error: Directory $5 does not exist."
    exit 1
fi

#ssh into router, run limitation script with arguments from this script
echo "applying limits at router"
ssh -t tobias@10.0.0.1 "sudo bash ~/Code/tobias-master-thesis-webrtc/utils/tc/limit_router.sh $1 $2 $3"

sudo tcpdump -i enp0s31f6 -w $out_dir/if_dump.pcap &
sleep 5

#run chromium and tcpdump for some time before exiting
timeout $4 bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_chromium.sh $out_dir https://$SENDER:8888?$6
sudo killall tcpdump

