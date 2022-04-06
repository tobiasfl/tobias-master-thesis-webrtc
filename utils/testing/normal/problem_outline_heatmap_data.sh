#!/bin/bash

BW_START=5
BW_END=10
for ((bw=$BW_START;bw<=$BW_END;bw++)); do
    out_dir=problem_outline_data_${bw}mbps$(date --rfc-3339=date)
    mkdir $out_dir
    
    RUNS=10

    for ((run=1;run<=$RUNS;run++)); do
        sub_dir=$out_dir/run$run
        mkdir $sub_dir

        divide="(50*$bw)"
        by="(1.276*8.0)"
        bdp_pkts=$(echo "($divide+$by-1)/$by" | bc -q -l)
        bdp_pkts=$(echo ${bdp_pkts%%.*})
        echo "bw:  $bw"
        echo "run: $run"
        echo "queue length: $bdp_pkts"

        bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh $bw 50 $bdp_pkts 120 $sub_dir 'rtp1start=0\&sctp1start=0' 'none' 'chromium'

    done
    
    zip -r $out_dir.zip $out_dir
    rm $out_dir -rf
done


