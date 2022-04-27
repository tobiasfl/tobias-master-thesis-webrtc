OUT_DIR=sctp_vs_gcc_equal$(date --rfc-3339=date)

mkdir $OUT_DIR

CAPACITY_MBPS=2
RTT=100
QUEUE_PKTS=60

TEST_LEN=125

run_experiment () {
    mkdir $2
    bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh $CAPACITY_MBPS $RTT $QUEUE_PKTS $TEST_LEN $2 'rtp1start=10000\&rtp1end=120000\&sctp1start=0\&sctp1end=120000' "DesiredRateFseNgPaper,equal,$1"
    bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh $CAPACITY_MBPS $RTT $QUEUE_PKTS $TEST_LEN $2 'rtp1start=0\&rtp1end=120000\&sctp1start=10000\&sctp1end=120000' "DesiredRateFseNgPaper,equal,$1"
    sleep 1
}

for RUN in "1" "2" "3" "4" "5" "6" "7" "8" "9" "10"
do
    run_experiment "FseNg" $OUT_DIR/run$RUN
    run_experiment "FseNgV2" $OUT_DIR/run$RUN
    run_experiment "FseV2" $OUT_DIR/run$RUN
done



