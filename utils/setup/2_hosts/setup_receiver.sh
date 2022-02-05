IF=enp0s31f6

#configure interface
ip link set dev $IF up
ip addr add dev $IF 192.168.0.2/24
ip addr list $IF

#turn off offloading
bash ~/Code/tobias-master-thesis-webrtc/utils/setup/turn_off_offloading.sh $IF

#start ssh service
systemctl start sshd.service
