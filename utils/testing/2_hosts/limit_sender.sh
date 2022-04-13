#!/bin/bash -x


if [ $# -lt 2 ]; then
    echo "usage: $0 <capacity(Mbps)> <queue_limit(pkts)>"
    exit 1
fi

IF=enp0s31f6
RATE_MBIT=$1
PKT_LIMIT=$2

for iface in $IF; do
    tc qdisc del dev $iface root
    tc qdisc add dev $iface root handle 1: htb default 1
    tc class add dev $iface parent 1: classid 1:1 htb rate ${RATE_MBIT}mbit ceil ${RATE_MBIT}mbit 
    tc qdisc add dev $iface parent 1:1 handle 1001: pfifo limit ${PKT_LIMIT} 
    tc filter add dev $iface protocol ip parent 1: prio 1 matchall flowid 1:1
done
