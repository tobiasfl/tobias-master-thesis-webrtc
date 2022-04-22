#!/bin/bash
RECEIVER=192.168.0.2
SENDER=10.0.0.2

SS_LEN=55
TEST_LEN=60
TCP_START_TIME=20

out_dir=sanity_check$(date --rfc-3339=date)

url="https://$SENDER:8888?rtp1start=0\&sctp1start=10000"

enabled_features="none"

CHROMIUM_OVERRIDE="chromium"
CHROMIUM_OVERRIDE=""

TCP_ALGO=reno

RTT=100
OWD=$(($RTT / 2))
BW=2
QUEUE_PKTS=17

mkdir $out_dir

sudo echo "applying limits at receiver"
#ssh -t tobias@$RECEIVER "sudo bash ~/Code/tobias-master-thesis-webrtc/utils/testing/2_hosts/setup_ingress_delay_limit.sh enp0s31f6 $RTT"
ssh -t tobias@$RECEIVER "sudo bash ~/Code/tobias-master-thesis-webrtc/utils/testing/2_hosts/setup_ingress_rate_limit.sh enp0s31f6 $BW $RTT $QUEUE_PKTS"

#echo "applying limits at sender"
#sudo bash ~/Code/tobias-master-thesis-webrtc/utils/testing/2_hosts/setup_ingress_bw_and_q_limit.sh enp0s31f6 $BW $QUEUE_PKTS
#sudo bash ~/Code/tobias-master-thesis-webrtc/utils/testing/2_hosts/setup_ingress_rate_limit.sh enp0s31f6 $BW $OWD $QUEUE_PKTS

read -p "start iperf server at receiver"

sudo tcpdump -i enp0s31f6 -w $out_dir/if_dump.pcap &

#timeout $SS_LEN bash ~/Code/tobias-master-thesis-webrtc/utils/ss/ss_output.sh $RECEIVER $out_dir &

#(sleep $TCP_START_TIME && iperf -c $RECEIVER -Z $TCP_ALGO -n 1000M)&

#For running tcp alone
#timeout $TEST_LEN iperf -c $RECEIVER -Z $TCP_ALGO -n 1000M

timeout $TEST_LEN bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_chromium.sh $out_dir $url $enabled_features $CHROMIUM_OVERRIDE

sudo killall tcpdump iperf

