#!/bin/bash
ROUTER=10.0.0.1
RECEIVER=192.168.0.2
SENDER=10.0.0.2

out_dir=sanity_check$(date --rfc-3339=date)

mkdir $out_dir

sudo echo "applying limits at router"
ssh -t tobias@$ROUTER "sudo bash ~/Code/tobias-master-thesis-webrtc/utils/tc/teardown_aqm.sh"
ssh -t tobias@$ROUTER "sudo bash ~/Code/tobias-master-thesis-webrtc/utils/tc/j_limit_router.sh 5 50 25"

read -p "start iperf server at receiver"

sudo tcpdump -i enp0s31f6 -w $out_dir/if_dump.pcap &

#start iperf at sender 
iperf -c $RECEIVER -Z reno -n 1000M &

#logging_flags="--enable-logging=stderr --vmodule=*/webrtc/*=1"

CHROMIUM_SRC=~/Code/chromium/src
CHROMIUM_BIN=$CHROMIUM_SRC/out/Default/chrome
CHROMIUM_BIN=chromium

url="https://$SENDER:8888?rtp1start=0"

timeout 120 $CHROMIUM_BIN --use-fake-device-for-media-stream --use-file-for-fake-video-capture=$CHROMIUM_SRC/KristenAndSara_1280x720_60.y4m  --force-fieldtrials=WebRTC-DataChannel-Dcsctp/Disabled $logging_flags $url > $out_dir/log.txt 2>&1

sudo killall tcpdump iperf

