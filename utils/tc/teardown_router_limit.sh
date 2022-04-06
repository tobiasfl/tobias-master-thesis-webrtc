IFSND=enx00e096660248
IFRCV=enp3s0
IFVIR=ifb0


tc qdisc del dev ${IFRCV} root
tc qdisc del dev ${IFVIR} root 
tc qdisc del dev ${IFSND} ingress
