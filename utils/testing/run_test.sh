#!/bin/bash
ROUTER=10.0.0.1
RECEIVER=192.168.0.2
SENDER=10.0.0.2

if [ $# -lt 7 ]; then
    echo "USAGE: $0 <mbit/s> <delay ms> <queue length pkts> <run length sec.> <out dir> <web page query> <feature list with commas between> [browser bin override]"
    echo "***POSSIBLE FEATURES:***"
    echo "fse versions: [none|Fse|FseNg|FseV2|FseNgV2]"
    echo "desired rate cases: [infinity|DesiredRateFseNgPaper]"
    echo "priority cases: [equal|PriorityRateFlowDouble|PriorityCwndFlowDouble]"
    echo "FseNg choices: {UpdateValFinalRate}"
    exit 1
fi

enabled_features_list=$7

out_dir=$5/$1mbit_$2ms_$3pkts_$4sec_$6_$enabled_features_list$8

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
ssh -t tobias@$ROUTER "sudo bash ~/Code/tobias-master-thesis-webrtc/utils/tc/teardown_aqm.sh $1 $2 $3"
ssh -t tobias@$ROUTER "sudo bash ~/Code/tobias-master-thesis-webrtc/utils/tc/j_limit_router.sh $1 $2 $3"

#Start data collection
echo "starting tcpdump at receiver"
ssh -tt tobias@$RECEIVER "sudo tcpdump -i enp0s31f6 -w /home/tobias/Code/test_results/recv_if_dump.pcap" & #1>/dev/null&
PID_TCPDUMP_RECEIVER=$!

#echo "starting tcpdump at sender"
sudo tcpdump -i enp0s31f6 -w $out_dir/if_dump.pcap &

sleep 3

timeout $4 bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_chromium.sh $out_dir https://$SENDER:8888?$6 $enabled_features_list $8

kill $PID_TCPDUMP_RECEIVER
ssh -t tobias@$RECEIVER "sudo killall tcpdump"
sudo killall tcpdump

#retrieve tcpdump
echo "retrieving pcap from receiver"
scp $RECEIVER:/home/tobias/Code/test_results/recv_if_dump.pcap "$out_dir/recv_if_dump.pcap"

