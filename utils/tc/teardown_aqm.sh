#!/bin/bash

for iface in "enx00e096660248" "enp3s0"; do
    tc qdisc del dev $iface root
done
