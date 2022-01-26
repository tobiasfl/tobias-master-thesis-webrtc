IF=enp0s31f6

#configure interface
ip link set dev $IF up
ip addr add dev $IF 192.168.0.2/24
ip addr list $IF

#add static route to other subnet
ip route add 10.0.0.0/24 via 192.168.0.1 dev enp0s31f6



#turn off offloading
bash turn_off_offloading.sh $IF

#start ssh service
systemctl start sshd.service
