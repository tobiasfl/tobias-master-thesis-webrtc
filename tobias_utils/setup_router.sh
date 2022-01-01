IFA=enp0s31f6
IFB=enp0s20f0u1

echo 1 > /proc/sys/net/ipv4/ip_forward

iptables -F
iptables -t nat -F

iptables -A FORWARD -i $IFA -o $IFB -j ACCEPT
iptables -A FORWARD -i $IFB -o $IFA -j ACCEPT
