IF=enp0s31f6
DELAY=50ms
BW=3mbit
QUEUELIMIT=12

tc qdisc del dev $IF root

tc qdisc add dev $IF root handle 1: htb default 1
tc class add dev $IF parent 1: classid 1:1 htb rate $BW ceil $BW 

#limit is maximum number of packets the qdisc may hold at a time
tc qdisc add dev $IF parent 1:1 handle 1001: netem limit $QUEUELIMIT delay $DELAY loss 0.0%

tc filter add dev $IF protocol ip parent 1: prio 1 matchall flowid 1:1
