#!/bin/bash

if [ $# -lt 1 ]; then
    echo "USAGE: $0 <output log dir>"
    exit 1
fi


if [ ! -d $1 ]; then
    echo "Error: Directory $1 does not exist."
    exit 1
fi

~/Code/chromium/src/out/Default/chrome --no-sandbox --use-fake-device-for-media-stream --use-file-for-fake-video-capture=KristenAndSara_1280x720_60.y4m --enable-logging=stderr --log_level=0 --v=3 --force-fieldtrials=WebRTC-Debugging-RtpDump/Enabled  https://localhost:8888 > $1/log.txt 2>&1

#This enables SRTP packet dumping
#--enable-logging --v=3 --force-fieldtrials=WebRTC-Debugging-RtpDump/Enabled
