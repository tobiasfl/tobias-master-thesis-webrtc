IF=lo
DELAY=25ms
BW=10mbit

tc qdisc del dev $IF root

#sets up root qdisc with a handle name 1:0 and delay
tc qdisc add dev $IF root handle 1:0 netem delay $DELAY 

tc qdisc add dev $IF parent 1:1 handle 10: tbf rate $BW burst 80kbit latency 400ms 
