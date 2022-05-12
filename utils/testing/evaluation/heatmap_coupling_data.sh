#!/bin/bash

#TODO: this one has to be reconsidered, because SCTP can not reach high tput of 8 while running modiifed chromium
#3,4,5,6,7,8,9,10
for bw in "8"
do
    #for BDP_TIMES in "0.5" "1" "2" "3"
    for BDP_TIMES in "3"
    do
        out_dir=problem_outline_data_${bw}mbps_${BDP_TIMES}bdps$(date --rfc-3339=date)
        mkdir $out_dir

        for run in "1" "2" "3" "4" "5" "6" "7" "8" "9" "10"
        do
            sub_dir=$out_dir/run$run
            mkdir $sub_dir

            divide="(100*$bw)"
            by="(1.276*8.0)"
            bdp_pkts=$(echo "($divide+$by-1)/$by" | bc -q -l)
            bdp_pkts=$(echo ${bdp_pkts%%.*})
            bdp_pkts=$(echo "$bdp_pkts*$BDP_TIMES" | bc -q)
            echo "bw:  $bw"
            echo "run: $run"
            echo "queue length: $bdp_pkts"

            bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh $bw 100 $bdp_pkts 120 $sub_dir 'rtp1start=0\&sctp1start=0' 'FseV2,DesiredRateFseCase2,equal'

        done
        
        zip -r $out_dir.zip $out_dir
        rm $out_dir -rf
    done
done


