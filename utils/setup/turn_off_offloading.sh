if [ $# -eq 0 ]; then
    echo "Usage: turn_off_offloading.sh <if>"
    exit 1
fi

ethtool -K $1 gro off
ethtool -K $1 tso off
ethtool -K $1 lso off
ethtool -K $1 lro off
ethtool -K $1 sg off
ethtool -K $1 ufo off
ethtool -K $1 gso off
ethtool -K $1 rxvlan off
ethtool -K $1 txvlan off
ethtool -K $1 rxhash off
ethtool -K $1 tx-checksum-ipv4 off
ethtool -K $1 tx-checksum-ip-generic off
ethtool -K $1 rx-checksum off

