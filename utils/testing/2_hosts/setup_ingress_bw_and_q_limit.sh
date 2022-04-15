#!/bin/bash

if [ $# -lt 3 ]; then
	echo "usage: $0 <interface> <capacity(Mbps)> <queue_limit(pkts)>"
	exit 1
fi

tc filter del dev $1
tc qdisc del dev $1 ingress
ip link set dev ifb0 down
modprobe -r ifb

sleep 1

modprobe ifb numifbs=1
ip link set dev ifb0 up
tc qdisc add dev $1 ingress
tc filter add dev $1 parent ffff: protocol ip u32 match u32 0 0 flowid 1:1 action mirred egress redirect dev ifb0

tc qdisc del dev ifb0 root
tc qdisc add dev ifb0 root handle 1: htb default 10
tc class add dev ifb0 parent 1: classid 10 htb rate "$2"mbit burst 1500
tc qdisc add dev ifb0 parent 1:10 pfifo limit "$3"
