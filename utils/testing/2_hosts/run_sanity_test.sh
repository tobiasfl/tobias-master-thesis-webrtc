#!/bin/bash
RECEIVER=192.168.0.2
SENDER=10.0.0.2

SS_LEN=49
TEST_LEN=50
TCP_START_TIME=0

out_dir=sanity_check$(date --rfc-3339=date)

url="https://$SENDER:8888?rtp1start=0"

enabled_features="none"

CHROMIUM_OVERRIDE="chromium"

RTT=100
OWD=$(($RTT / 2))

mkdir $out_dir

sudo echo "applying limits at receiver"
ssh -t tobias@$RECEIVER "sudo bash ~/Code/tobias-master-thesis-webrtc/utils/testing/2_hosts/setup_ingress_rate_limit.sh enp0s31f6 2 $OWD 17"

echo "applying limits at sender"
sudo bash ~/Code/tobias-master-thesis-webrtc/utils/testing/2_hosts/setup_ingress_rate_limit.sh enp0s31f6 2 $OWD 17

read -p "start iperf server at receiver"

sudo tcpdump -i enp0s31f6 -w $out_dir/if_dump.pcap &

timeout $SS_LEN bash ~/Code/tobias-master-thesis-webrtc/utils/ss/ss_output.sh $RECEIVER $out_dir &

(sleep $TCP_START_TIME && iperf -c $RECEIVER -Z reno -n 1000M)&

#timeout $TEST_LEN bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_chromium.sh $out_dir $url $enabled_features $CHROMIUM_OVERRIDE
sleep $TEST_LEN

sudo killall tcpdump iperf

