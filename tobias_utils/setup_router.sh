IFA=enp3s0
IFB=enx00e096660248

echo 1 > /proc/sys/net/ipv4/ip_forward

iptables -F
iptables -t nat -F

iptables -A FORWARD -i $IFA -o $IFB -j ACCEPT
iptables -A FORWARD -i $IFB -o $IFA -j ACCEPT
