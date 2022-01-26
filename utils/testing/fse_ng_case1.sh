#!/bin/bash

if [ $# -lt 1 ]; then
    echo "USAGE: $0 <output directory>"
    exit 1
fi

R1S=0
R2S=0
S1S=15000
R1E=40000
R2E=40000
S1E=70000
QUERYSTRING=rtp1start\=$R1S\&rtp2start\=$R2S\&sctp1start\=$S1S\&rtp1end\=$R1E\&rtp2end\=$R2E\&sctp1end\=$S1E

bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh '3.5' 50 100 90 $1 https://localhost:8888?$QUERYSTRING
