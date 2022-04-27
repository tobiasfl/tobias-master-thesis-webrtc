out_dir=debugging$(date --rfc-3339=date)

mkdir $out_dir

bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 2 100 51 120 $out_dir 'rtp1start=0\&sctp1start=10000' 'FseNgV2,infinity,PriorityCwndFlowDouble,'
#bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 50 15 60 $out_dir 'rtp1start=0\&rtp2start=5000' 'FseNgV2,infinity,equal,'
