IF=enp0s31f6

#configure interface
ip link set dev $IF up
ip addr add dev $IF 10.0.0.2/24
ip addr list $IF

#add default route through router
ip route add default via 10.0.0.1

#add static route to other subnet
ip route add 192.168.0.0/24 via 10.0.0.1 dev enp0s31f6

