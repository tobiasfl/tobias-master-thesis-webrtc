IFRECV=enp3s0
IFSND=enx00e096660248

#configure IFRECV
ip link set dev $IFRECV
ip addr add dev $IFRECV 192.168.0.1/24
ip addr list $IFRECV

#configure IFSND
ip link set dev $IFSND
ip addr add dev $IFSND 10.0.0.1/24
ip addr list $IFSND


#enable routing
echo net.ipv4.ip_forward=1 > /etc/sysctl.conf
sysctl -p /etc/sysctl.conf

#limit bandwidth when forwarding
DELAY_MS=10
RATE_MBIT=3
QUEUELIMIT=15

tc qdisc del dev $IFRECV root

tc qdisc add dev $IFRECV root handle 1: htb default 1 tc class add dev $IFRECV parent 1: classid 1:1 htb rate ${RATE_MBIT}mbit ceil ${RATE_MBIT}mbit 

#limit is maximum number of packets the qdisc may hold at a time
tc qdisc add dev $IFRECV parent 1:1 handle 1001: netem limit ${QUEUELIMIT} delay ${DELAY_MS}ms loss 0.0%

tc filter add dev $IFRECV protocol ip parent 1: prio 1 matchall flowid 1:1
