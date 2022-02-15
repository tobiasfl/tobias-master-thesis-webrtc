#!/bin/bash

usage() { echo "Usage: $0 <output log directory> <web page url> [-d]"; exit 1; }

if [ $# -lt 2 ]; then
    usage
fi

if [[ $# -eq 3 && "-d" != $3 ]]; then
    echo "Error: ${3} is not equal to -d"
    usage
fi


if [ ! -d $1 ]; then
    echo "Error: Directory $1 does not exist."
    exit 1
fi

CHROMIUM_SRC=~/Code/chromium/src

logging_flags="--enable-logging=stderr --vmodule=*/webrtc/*=1"
if [ $# -eq 3 ]; then
    echo "Enabling rtp packet dumping"
    logging_flags="${logging_flags} --force-fieldtrials=WebRTC-Debugging-RtpDump/Enabled"
fi 

#The run below will also dump both RTP and SCTP packets

#$CHROMIUM_SRC/out/Default/chrome --use-fake-device-for-media-stream --use-file-for-fake-video-capture=$CHROMIUM_SRC/KristenAndSara_1280x720_60.y4m --enable-logging=stderr --log_level=0 --v=3 $2 > $1/log.txt 2>&1

#The run below does not dump RTP and SCTP packets

$CHROMIUM_SRC/out/Default/chrome --use-fake-device-for-media-stream --use-file-for-fake-video-capture=$CHROMIUM_SRC/KristenAndSara_1280x720_60.y4m  --force-fieldtrials=WebRTC-DataChannel-Dcsctp/Disabled $logging_flags $2 > $1/log.txt 2>&1

#To make sure dcsctptransport is used instead of usrsctptransport
#--force-fieldtrials=WebRTC-DataChannel-Dcsctp/Enabled

#This enables SRTP packet dumping
#--enable-logging --v=3 --force-fieldtrials=WebRTC-Debugging-RtpDump/Enabled
