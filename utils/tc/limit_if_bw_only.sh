IF=enp0s31f6
DELAY_MS=10
RATE_MBIT=3
QUEUELIMIT=15
#BDP_BYTES=$(echo "($DELAY_MS/1000.0)*($RATE_MBIT*1000000.0/8.0)" | bc -q -l)
#BDP_PKTS=(echo "$BDP_BYTES/1250" | bc -q)
#LIMIT_PKTS=$(echo "$BDP_PKTS+$QUEUELIMIT" | bc -q)

tc qdisc del dev $IF root

tc qdisc add dev $IF root handle 1: htb default 1
tc class add dev $IF parent 1: classid 1:1 htb rate ${RATE_MBIT}mbit ceil ${RATE_MBIT}mbit 

tc filter add dev $IF protocol ip parent 1: prio 1 matchall flowid 1:1
