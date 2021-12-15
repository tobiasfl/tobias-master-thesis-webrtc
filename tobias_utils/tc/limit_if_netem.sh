IF=enp0s31f6
DELAY_MS=10
RATE_MBIT=3
QUEUELIMIT=15
BDP_BYTES=$(echo "($DELAY_MS/1000.0)*($RATE_MBIT*1000000.0/8.0)" | bc -q -l)
BDP_PKTS=$(echo "$BDP_BYTES/1250" | bc -q)
LIMIT_PKTS=$(echo "$BDP_PKTS+$QUEUELIMIT" | bc -q)

tc qdisc del dev $IF root

#inspired by "Rate control" from https://wiki.linuxfoundation.org/networking/netem
tc qdisc add dev $IF root netem delay ${DELAY_MS}ms rate ${RATE_MBIT}Mbit limit ${LIMIT_PKTS}
