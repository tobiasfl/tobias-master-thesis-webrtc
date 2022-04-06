#!/bin/bash -x

if [ $# -lt 3 ]; then
	echo "usage: $0 <capacity(Mbps)> <delay(ms)> <queue_limit(pkts)>"
	exit 1
fi

DELAY_MS=$(echo "$2/2" | bc -q)

for iface in "enx00e096660248" "enp3s0"; do
    tc qdisc del dev $iface root
    tc qdisc add dev $iface root handle 1: netem delay "$DELAY_MS"ms
    tc qdisc add dev $iface parent 1: handle 2: htb default 10
    tc class add dev $iface parent 2: classid 10 htb rate "$1"mbit burst 1500
    tc qdisc add dev $iface parent 2:10 handle 15: pfifo limit "$3"
done
