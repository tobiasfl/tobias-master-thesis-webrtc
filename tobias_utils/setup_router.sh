IFA=enp3s0
IFB=enx00e096660248

#configure IFA
ip link set dev $IFA
ip addr add dev $IFA 192.168.0.1/24
ip addr list $IFA

#configure IFB
ip link set dev $IFB
ip addr add dev $IFB 10.0.0.1/24
ip addr list $IFB


#enable routing
echo net.ipv4.ip_forward=1 > /etc/sysctl.conf
sysctl -p /etc/sysctl.conf
