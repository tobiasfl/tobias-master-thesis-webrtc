#!/bin/bash
ROUTER=10.0.0.1
RECEIVER=192.168.0.2
SENDER=10.0.0.2

SS_LEN=115
TEST_LEN=120
TCP_START_TIME=60

out_dir=sanity_check$(date --rfc-3339=date)

url="https://$SENDER:8888?rtp1start=0"

enabled_features="none"

CHROMIUM_OVERRIDE="chromium"


mkdir $out_dir

sudo echo "applying limits at router"
ssh -t tobias@$ROUTER "sudo bash ~/Code/tobias-master-thesis-webrtc/utils/tc/teardown_aqm.sh"
ssh -t tobias@$ROUTER "sudo bash ~/Code/tobias-master-thesis-webrtc/utils/tc/j_limit_router.sh 4 200 67"

read -p "start iperf server at receiver"

sudo tcpdump -i enp0s31f6 -w $out_dir/if_dump.pcap &

timeout $SS_LEN bash ~/Code/tobias-master-thesis-webrtc/utils/ss/ss_output.sh $RECEIVER $out_dir &

#for only running tcp solo
#timeout $TEST_LEN iperf -c $RECEIVER -Z reno -n 1000M

(sleep $TCP_START_TIME && iperf -c $RECEIVER -Z reno -n 1000M)&

timeout $TEST_LEN bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_chromium.sh $out_dir $url $enabled_features $CHROMIUM_OVERRIDE

sudo killall tcpdump iperf

