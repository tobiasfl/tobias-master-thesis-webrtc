#!/bin/bash

if [ $# -lt 2 ]; then
    echo "USAGE: $0 <output log dir> <web page url>"
    exit 1
fi
#TODO: add option of running with or without packet dumping

if [ ! -d $1 ]; then
    echo "Error: Directory $1 does not exist."
    exit 1
fi

CHROMIUM_SRC=~/Code/chromium/src

#TODO: try with --vmodule=*/webrtc/*=1 to get webrtc related logging only
#$CHROMIUM_SRC/out/Default/chrome --use-fake-device-for-media-stream --use-file-for-fake-video-capture=$CHROMIUM_SRC/KristenAndSara_1280x720_60.y4m --enable-logging=stderr --log_level=0 --v=3 $2 > $1/log.txt 2>&1

#The run below will also dump both RTP and SCTP packets

$CHROMIUM_SRC/out/Default/chrome --use-fake-device-for-media-stream --use-file-for-fake-video-capture=$CHROMIUM_SRC/KristenAndSara_1280x720_60.y4m --enable-logging=stderr --vmodule=*/webrtc/*=1 --force-fieldtrials=WebRTC-DataChannel-Dcsctp/Disabled  $2 > $1/log.txt 2>&1

#To make sure usrsctptransport is used instead of dcsctptransport
#--force-fieldtrials=WebRTC-DataChannel-Dcsctp/Enabled

#This enables SRTP packet dumping
#--enable-logging --v=3 --force-fieldtrials=WebRTC-Debugging-RtpDump/Enabled
