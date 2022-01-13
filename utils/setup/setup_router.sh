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

#limit stuff
bash ../tc/router.sh

#turn off offloading
bash turn_off_offloading.sh $IFSND
bash turn_off_offloading.sh $IFRECV
