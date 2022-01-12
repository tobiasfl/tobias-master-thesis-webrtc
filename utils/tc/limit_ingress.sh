IF=enp0s31f6
VIF=ifb0
DELAY_MS=10
RATE_MBIT=3
QUEUELIMIT=15
#BDP_BYTES=$(echo "($DELAY_MS/1000.0)*($RATE_MBIT*1000000.0/8.0)" | bc -q -l)
#BDP_PKTS=(echo "$BDP_BYTES/1250" | bc -q)
#LIMIT_PKTS=$(echo "$BDP_PKTS+$QUEUELIMIT" | bc -q)

modprobe ifb

ip link set dev $VIF up

tc qdisc add dev $IF ingress

tc filter add dev $IF parent ffff: \
    protocol ip u32 match u32 0 0 flowid 1:1 action mirred egress redirect dev $VIF

tc qdisc add dev $VIF root handle 1: htb default 1
tc class add dev $VIF parent 1: classid 1:1 htb rate ${RATE_MBIT}mbit ceil ${RATE_MBIT}mbit 
tc qdisc add dev $VIF parent 1:1 handle 1001: netem limit ${QUEUELIMIT} delay ${DELAY_MS}ms loss 0.0%


