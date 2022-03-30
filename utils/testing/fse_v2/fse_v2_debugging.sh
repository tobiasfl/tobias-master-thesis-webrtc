out_dir=fse_v2_debugging$(date --rfc-3339=date)

mkdir $out_dir



#bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 50 15 50 $out_dir 'rtp1start=0\&sctp1start=5000' 'FseV2,infinity,equal'
#bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 50 15 50 $out_dir 'rtp1start=0\&rtp2start=5000\&sctp1start=10000' 'FseV2,infinity,equal'
#bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 5 50 28 50 $out_dir 'rtp1start=0\&rtp2start=5000\&sctp1start=10000' 'FseV2,infinity,equal'
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 5 50 28 50 $out_dir 'rtp1start=0\&sctp1start=10000' 'FseV2,infinity,equal'

