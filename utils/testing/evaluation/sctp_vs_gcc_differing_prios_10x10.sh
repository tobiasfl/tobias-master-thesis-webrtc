OUT_DIR=sctp_vs_gcc_differing_prios_10x10_$(date --rfc-3339=date)

mkdir $OUT_DIR

CAPACITY_MBPS=2
RTT=100
QUEUE_PKTS=60

TEST_LEN=125


run_experiment () {
    mkdir $2
    PRIO_STR=PriorityEvaluationPlot$3
    echo $PRIO_STR
    bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh $CAPACITY_MBPS $RTT $QUEUE_PKTS $TEST_LEN $2 'rtp1start=0\&rtp1end=120000\&sctp1start=10000\&sctp1end=120000' "infinity,$PRIO_STR,$1"
    sleep 1
}

#going downwards from 10
for PRIO in  "7"
do
    OUT_DIR=$OUT_DIR/prio_case$PRIO
    mkdir $OUT_DIR
    for RUN in "1" "2" "3" "4" "5" "6" "7" "8" "9" "10"  
    do
        #run_experiment "FseNg" $OUT_DIR/run$RUN $PRIO TODO: have to fix FseNG and then run again
        run_experiment "FseNgV2" $OUT_DIR/run$RUN $PRIO
        run_experiment "FseV2" $OUT_DIR/run$RUN $PRIO
    done
    zip -r $OUT_DIR.zip $OUT_DIR
    rm $OUT_DIR -rf
done


