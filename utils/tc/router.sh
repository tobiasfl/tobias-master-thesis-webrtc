IFS=enx00e096660248
IFR=enp3s0
DELAY_MS=50
RATE_MBIT=3
QUEUELIMIT=15

modprobe ifb

ip link set dev ifb1 up
ip link set dev ifb0 up

tc qdisc del dev ifb0 root handle 1 htb
tc qdisc add dev ifb0 root handle 1 htb

tc qdisc del dev ifb1 root handle 2 htb
tc qdisc add dev ifb1 root handle 2 htb

iptables -t mangle -F
iptables -t mangle -A POSTROUTING -j MARK --set-mark 0

tc class add dev ifb0 parent 1: classid 1:1 htb rate 3mbit ceil 3mbit
tc qdisc add dev ifb0 parent 1:1 handle 1001: pfifo limit 15
tc filter add dev ifb0 protocol ip parent 1: handle 1 fw flowid 1:1 action mirred egress redirect dev ifb1

tc class add dev ifb1 parent 2: classid 2:1 htb rate 1000mbit ceil 1000mbit
tc qdisc add dev ifb1 parent 2:1 handle 1002: netem limit 1000 delay 50ms loss 0.0%
tc filter add dev ifb1 protocol ip parent 2: prio 1 handle 2 fw flowid 1:1 

iptables -t mangle -A POSTROUTING -s 10.0.0.2/24 -d 192.168.0.2/24 -j MARK --set-mark 1
