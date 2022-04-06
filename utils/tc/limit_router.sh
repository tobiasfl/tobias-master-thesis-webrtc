#!/bin/bash

if [ $# -lt 3 ]; then
    echo "USAGE: $0 <bandwidth mbit/s limit> <delay ms> <queue length pkts>"
    exit 1
fi

IFSND=enx00e096660248
IFRCV=enp3s0
IFVIR=ifb0

RATE_MBIT=$1
DELAY_MS=$2

#PKT_MTU=1267
#BDP_BYTES=$(echo "($DELAY_MS/1000.0)*($RATE_MBIT*1000000.0/8.0)" | bc -q -l)
#BDP_PKTS=(echo "$BDP_BYTES/1250" | bc -q)

PKT_LIMIT=$3

modprobe ifb

ip link set dev ${IFVIR} up

#send incoming packets on sender side to IFVIR
tc qdisc del dev ${IFSND} ingress
tc qdisc add dev ${IFSND} ingress
tc filter add dev ${IFSND} parent ffff: \
	protocol ip u32 match u32 0 0 flowid 1:1 action mirred egress redirect dev ${IFVIR}

#add delay to incoming packets
tc qdisc del dev ${IFVIR} root 
tc qdisc add dev ${IFVIR} root handle 1: htb default 1
tc class add dev ${IFVIR} parent 1: classid 1:1 htb rate 1000mbit ceil 1000mbit 
tc qdisc add dev ${IFVIR} parent 1:1 handle 1001: netem limit 1000 delay ${DELAY_MS}ms loss 0.0%

#limit bandwidth and queue size on outgoing packets
tc qdisc del dev ${IFRCV} root
tc qdisc add dev ${IFRCV} root handle 1: htb default 1
tc class add dev ${IFRCV} parent 1: classid 1:1 htb rate ${RATE_MBIT}mbit ceil ${RATE_MBIT}mbit 
tc qdisc add dev ${IFRCV} parent 1:1 handle 1001: pfifo limit ${PKT_LIMIT} 
tc filter add dev ${IFRCV} protocol ip parent 1: prio 1 matchall flowid 1:1

