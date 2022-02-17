#!/bin/bash
ROUTER=10.0.0.1
RECEIVER=192.168.0.2
SENDER=10.0.0.2

if [ $# -lt 7 ]; then
    echo "USAGE: $0 <mbit/s> <delay ms> <queue length pkts> <run length sec.> <out dir> <web page query> <feature list with commas between>"
    echo "***POSSIBLE FEATURES:***"
    echo "fse versions: [none|Fse|FseNg]"
    echo "desired rate cases: [infinity|DesiredRateFseNgPaper]"
    echo "priority cases: [equal|PriorityRateFlowDouble|PriorityCwndFlowDouble]"
    echo "FseNg choices: {UpdateValFinalRate}"
    exit 1
fi

enabled_features_list=$7

out_dir=$5/$1mbit_$2ms_$3pkts_$4sec_$6_$enabled_features_list

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
#(sudo so we don't need it when tcpdumping)
sudo echo "applying limits at router"
ssh -t tobias@10.0.0.1 "sudo bash ~/Code/tobias-master-thesis-webrtc/utils/tc/limit_router.sh $1 $2 $3"

sudo tcpdump -i enp0s31f6 -w $out_dir/if_dump.pcap &

#run chromium and tcpdump for some time before exiting
timeout $4 bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_chromium.sh $out_dir https://$SENDER:8888?$6 $enabled_features_list
sudo killall tcpdump

