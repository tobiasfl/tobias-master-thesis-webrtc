#!/bin/bash

usage() { echo "Usage: $0 <output log directory> <web page url> <enabled_features list>"; exit 1; }

if [ $# -lt 3 ]; then
    usage
fi

if [ ! -d $1 ]; then
    echo "Error: Directory $1 does not exist."
    exit 1
fi

CHROMIUM_SRC=~/Code/chromium/src
chromium_bin=${CHROMIUM_SRC}/out/Default/chrome

logging_flags="--enable-logging=stderr --vmodule=*/webrtc/*=1"

enabled_features="--enable-features=${3}"

sctp_lib="--force-fieldtrials=WebRTC-DataChannel-Dcsctp/Disabled"
#sctp_lib="--force-fieldtrials=WebRTC-DataChannel-Dcsctp/Enabled"


if [ -z "$4" ]; then
    echo "using default chromium build"
else
    echo "browser override, using {$4} and turning off logging"
    chromium_bin=$4
    logging_flags=''
    enabled_features=''
    #sctp_lib=''
fi

echo $enabled_features
#The run below does not dump RTP and SCTP packets
$chromium_bin --use-fake-device-for-media-stream --use-file-for-fake-video-capture=$CHROMIUM_SRC/KristenAndSara_1280x720_60.y4m $logging_flags $sctp_lib $enabled_features $2 > $1/log.txt 2>&1
