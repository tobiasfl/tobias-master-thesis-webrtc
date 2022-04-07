if [ $# -eq 0 ]; then
    echo "Usage: turn_off_offloading.sh <if>"
    exit 1
fi

ethtool -K $1 gro off
ethtool -K $1 tso off
ethtool -K $1 sg off
#ethtool -K $1 lro off
ethtool -A $1 autoneg off tx off rx off
#ethtool -K $1 ufo off
#ethtool -K $1 gso off
ethtool -K $1 tx-gso-partial off
