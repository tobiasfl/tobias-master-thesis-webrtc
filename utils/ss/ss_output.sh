#!/bin/bash

usage() { echo "Usage: $0 <dst address> <out dir> "; exit 1; }

if [ $# -lt 2 ]; then
    usage
fi


DST=$1
OUT_DIR=$2
OUT_TXT=$OUT_DIR/sender-ss.txt
OUT_CSV=$OUT_DIR/sender-ss.csv

touch $OUT_TXT

rm -f $OUT_TXT 

cleanup ()
{
	# get timestamp
	ts=$(cat $OUT_TXT |   sed -e ':a; /<->$/ { N; s/<->\n//; ba; }' | grep "ESTAB"  |  grep "unacked" |  awk '{print $1}')

	# get sender
	sender=$(cat $OUT_TXT |   sed -e ':a; /<->$/ { N; s/<->\n//; ba; }' | grep "ESTAB"  | grep "unacked" | awk '{print $6}')


	# retransmissions - current, total
	retr=$(cat $OUT_TXT |   sed -e ':a; /<->$/ { N; s/<->\n//; ba; }' | grep "ESTAB"  |  grep -oP '\bunacked:.*\brcv_space'  | awk -F '[:/ ]' '{print $4","$5}' | tr -d ' ')


	# get cwnd, ssthresh
	cwn=$(cat $OUT_TXT |   sed -e ':a; /<->$/ { N; s/<->\n//; ba; }' | grep "ESTAB"    |  grep "unacked" | grep -oP '\bcwnd:.*(\s|$)\bbytes_acked' | awk -F '[: ]' '{print $2","$4}')


        # get rtt                                                                                                             #\b means left edge is non word char
                                                                                                                              #then *(\s|$) means catch group of 
	rtt=$(cat $OUT_TXT |   sed -e ':a; /<->$/ { N; s/<->\n//; ba; }' | grep "ESTAB"    |  grep "unacked" | grep -oP '\brtt:.*(\s|$)\bmss' | awk -F '[:/ ]' '{print $2}')

	# concatenate into one CSV
        paste -d ',' <(printf %s "$ts") <(printf %s "$sender") <(printf %s "$retr") <(printf %s "$cwn") <(printf %s "$rtt") > $OUT_CSV

	exit 0
}

trap cleanup SIGINT SIGTERM

while [ 1 ]; do 
	ss --no-header -ein dst $DST | ts '%.s' | tee -a $OUT_TXT 
done
