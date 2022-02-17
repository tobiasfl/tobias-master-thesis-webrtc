#!/bin/bash

usage() { echo "Usage: $0 <output log directory> <web page url> <enabled_features list>"; exit 1; }

if [ $# -lt 3 ]; then
    usage
fi

#if [[ $# -eq 3 && "-d" != $3 ]]; then
#    echo "Error: ${3} is not equal to -d"
#    usage
#fi


if [ ! -d $1 ]; then
    echo "Error: Directory $1 does not exist."
    exit 1
fi

CHROMIUM_SRC=~/Code/chromium/src

logging_flags="--enable-logging=stderr --vmodule=*/webrtc/*=1"
#if [ $# -eq 3 ]; then
#    echo "Enabling rtp packet dumping"
#    logging_flags="${logging_flags} --force-fieldtrials=WebRTC-Debugging-RtpDump/Enabled"
#fi 

enabled_features="--enable-features=${3}"

echo $enabled_features
#The run below does not dump RTP and SCTP packets
$CHROMIUM_SRC/out/Default/chrome --use-fake-device-for-media-stream --use-file-for-fake-video-capture=$CHROMIUM_SRC/KristenAndSara_1280x720_60.y4m  --force-fieldtrials=WebRTC-DataChannel-Dcsctp/Disabled $logging_flags $enabled_features $2 > $1/log.txt 2>&1

#To make sure dcsctptransport is used instead of usrsctptransport
#--force-fieldtrials=WebRTC-DataChannel-Dcsctp/Enabled
