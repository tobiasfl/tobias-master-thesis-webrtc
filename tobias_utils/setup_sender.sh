IF=enp0s31f6

#configure interface
ip link set dev $IF up
ip addr add dev $IF 10.0.0.2/24
ip addr list $IF

#add default route through router
ip route add default via 10.0.0.1
