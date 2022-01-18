
IFRCV=enp3s0
QUEUELIMIT=102

tc qdisc del dev ${IFRCV} root
tc qdisc add dev ${IFRCV} root pfifo limit ${QUEUELIMIT}
