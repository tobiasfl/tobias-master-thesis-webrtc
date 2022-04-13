#!/bin/bash -x

IF=enp0s31f6

if [ $# -lt 1 ]; then
	#echo "usage: $0 <capacity(Mbps)> <delay(ms)> <queue_limit(pkts)>"
	echo "usage: $0 <delay(ms)>"
	exit 1
fi

DELAY_MS=$1

for iface in $IF; do
    tc qdisc del dev $iface root
    tc qdisc add dev $iface root handle 1: netem delay "$DELAY_MS"ms
done
