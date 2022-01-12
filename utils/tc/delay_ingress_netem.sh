IF=enp0s31f6
DELAY_MS=10

modprobe ifb
ip link set dev ifb0 up
tc qdisc add dev $IF ingress
tc filter add dev $IF parent ffff: \
    protocol ip u32 match u32 0 0 flowid 1:1 action mirred egress redirect dev ifb0
tc qdisc add dev ifb0 root netem delay ${DELAY_MS}ms
