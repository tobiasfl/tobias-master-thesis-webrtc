#!/bin/bash
ROUTER=10.0.0.1
RECEIVER=192.168.0.2
SENDER=10.0.0.2

OUT_DIR=long_tcp_vs_sctp_vs_gcc$(date --rfc-3339=date)

mkdir $OUT_DIR

CAPACITY_MBPS=3.5
RTT=100
QUEUE_PKTS=90

TEST_LEN=125
TCP_START_TIME=20

ENABLED_FEATURES="infinity,equal"

URL="https://$SENDER:8888?rtp1start=0\&rtp1end=120000\&sctp1start=10000\&sctp1end=120000"

read -p "start iperf server at receiver and load application in chromium"

#TODO: must probably use iperf3 to make it work with bbr
run_experiment () {
    TEST_DIR=$OUT_DIR/${CAPACITY_MBPS}mbit_${RTT}ms_${QUEUE_PKTS}pkts_${TEST_LEN}s_${ENABLED_FEATURES}_$1_$2

    mkdir $TEST_DIR

    sudo echo "applying limits at router"
    ssh -t tobias@$ROUTER "sudo bash ~/Code/tobias-master-thesis-webrtc/utils/tc/teardown_aqm.sh"
    ssh -t tobias@$ROUTER "sudo bash ~/Code/tobias-master-thesis-webrtc/utils/tc/j_limit_router.sh $CAPACITY_MBPS $RTT $QUEUE_PKTS"

    echo "starting tcpdump at receiver"
    ssh -tt tobias@$RECEIVER "sudo tcpdump -i enp0s31f6 -w /home/tobias/Code/test_results/recv_if_dump.pcap" & #1>/dev/null&
    PID_TCPDUMP_RECEIVER=$!
    
    sudo tcpdump -i enp0s31f6 -w $TEST_DIR/if_dump.pcap &

    sleep 1

    timeout $TEST_LEN bash ~/Code/tobias-master-thesis-webrtc/utils/ss/ss_output.sh $RECEIVER $TEST_DIR &

    (sleep $TCP_START_TIME && iperf -c $RECEIVER -Z $2 -n 10000M)&


    timeout $TEST_LEN bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_chromium.sh $TEST_DIR $URL "$ENABLED_FEATURES,$1"

    kill $PID_TCPDUMP_RECEIVER
    ssh -t tobias@$RECEIVER "sudo killall tcpdump"
    sudo killall tcpdump iperf

    #retrieve tcpdump
    echo "retrieving pcap from receiver"
    ssh -t tobias@$ROUTER "sudo bash ~/Code/tobias-master-thesis-webrtc/utils/tc/teardown_aqm.sh"
    scp $RECEIVER:/home/tobias/Code/test_results/recv_if_dump.pcap "$TEST_DIR/recv_if_dump.pcap"

    #get stats dumped by web clients
    cp ~/Code/webrtc-test-app/1video_stats.txt $TEST_DIR
    rm ~/Code/webrtc-test-app/1video_stats.txt
    cp ~/Code/webrtc-test-app/1data_channel_stats.txt $TEST_DIR
    rm ~/Code/webrtc-test-app/1data_channel_stats.txt

    sleep 5
}

#run_experiment "none" "reno"
#run_experiment "FseNg" "reno"
#run_experiment "FseNgV2" "reno"
#run_experiment "FseV2" "reno"

#run_experiment "none" "cubic"
#run_experiment "FseNg" "cubic"
#run_experiment "FseNgV2" "cubic"
#run_experiment "FseV2" "cubic"

