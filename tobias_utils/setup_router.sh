IFA=enx00e096660248
IFB=enp3s0

#configure IFA
ip link set dev $IFA
ip addr add dev $IFA 192.168.0.1/24
ip addr list $IFA

#configure IFB
ip link set dev $IFA
ip addr add dev $IFA 10.0.0.1/24
ip addr list $IFA


#enable routing
echo net.ipv4.ip_forward=1 > /etc/sysctl.conf
sysctl -p /etc/sysctl.conf
