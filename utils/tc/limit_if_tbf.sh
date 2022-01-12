IF=enp0s31f6
DELAY=10ms
BW=3mbit
QUEUELIMIT=5

tc qdisc del dev $IF root

# latency is how long a packet can be buffered before it is thrown away, can be replaced with limit
tc qdisc add dev $IF handle 10: root tbf rate $BW burst 150kbit limit 150kbit 

#sets up root qdisc with a handle name 1:0 and delay
tc qdisc add dev $IF parent 10:1 handle 100: netem limit $QUEUELIMIT delay $DELAY loss 0.0%

