tc qdisc add dev ifb1 root handle 1 htb

iptables -t mangle -F
iptables -t mangle -A POSTROUTING -j MARK --set-mark 0

executing task 'init_host_custom'
executing task 'init_host_custom'
executing task 'init_host_custom'
executing task 'init_pipe'

tc class add dev ifb0 parent 1: classid 1:1 htb rate 200mbit ceil 200mbit
tc qdisc add dev ifb0 parent 1:1 handle 1001: pfifo limit 34
tc filter add dev ifb0 protocol ip parent 1: handle 1 fw flowid 1:1

tc class add dev 10Gb parent 1: classid 1:1 htb rate 1000mbit ceil 1000mbit
tc qdisc add dev 10Gb parent 1:1 handle 1001: netem limit 1000 delay 1ms loss 0.0%
tc filter add dev 10Gb protocol ip parent 1: handle 1 fw flowid 1:1 action mirred egress redirect dev ifb0

tc class add dev ifb1 parent 1: classid 1:1 htb rate 200mbit ceil 200mbit
tc qdisc add dev ifb1 parent 1:1 handle 1001: pfifo limit 34
tc filter add dev ifb1 protocol ip parent 1: handle 1 fw flowid 1:1

tc class add dev 10Ga parent 1: classid 1:1 htb rate 1000mbit ceil 1000mbit
tc qdisc add dev 10Ga parent 1:1 handle 1001: netem limit 1000 delay 1ms loss 0.0%
tc filter add dev 10Ga protocol ip parent 1: handle 1 fw flowid 1:1 action mirred egress redirect dev ifb1

iptables -t mangle -A POSTROUTING -s 172.16.10.0/24 -d 172.16.11.0/24 -j MARK --set-mark 1

executing task 'init_pipe'

tc class add dev ifb0 parent 1: classid 1:2 htb rate 200mbit ceil 200mbit
tc qdisc add dev ifb0 parent 1:2 handle 1002: pfifo limit 34
tc filter add dev ifb0 protocol ip parent 1: handle 2 fw flowid 1:2

tc class add dev 10Gb parent 1: classid 1:2 htb rate 1000mbit ceil 1000mbit
tc qdisc add dev 10Gb parent 1:2 handle 1002: netem limit 1000 delay 1ms loss 0.0%
tc filter add dev 10Gb protocol ip parent 1: handle 2 fw flowid 1:2 action mirred egress redirect dev ifb0

tc class add dev ifb1 parent 1: classid 1:2 htb rate 200mbit ceil 200mbit
tc qdisc add dev ifb1 parent 1:2 handle 1002: pfifo limit 34
tc filter add dev ifb1 protocol ip parent 1: handle 2 fw flowid 1:2

tc class add dev 10Ga parent 1: classid 1:2 htb rate 1000mbit ceil 1000mbit
tc qdisc add dev 10Ga parent 1:2 handle 1002: netem limit 1000 delay 1ms loss 0.0%
tc filter add dev 10Ga protocol ip parent 1: handle 2 fw flowid 1:2 action mirred egress redirect dev ifb1

iptables -t mangle -A POSTROUTING -s 172.16.11.0/24 -d 172.16.10.0/24 -j MARK --set-mark 2
