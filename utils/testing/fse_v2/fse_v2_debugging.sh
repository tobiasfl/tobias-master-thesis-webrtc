out_dir=fse_v2_debugging$(date --rfc-3339=date)

mkdir $out_dir



bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 25 8 50 $out_dir 'rtp1start=0\&sctp1start=10000' 'FseV2,infinity,equal'
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 50 15 50 $out_dir 'rtp1start=0\&sctp1start=10000' 'FseV2,infinity,equal'
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 100 30 50 $out_dir 'rtp1start=0\&sctp1start=10000' 'FseV2,infinity,equal'
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 200 60 50 $out_dir 'rtp1start=0\&sctp1start=10000' 'FseV2,infinity,equal'

bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 4 25 10 25 $out_dir 'rtp1start=0\&sctp1start=10000' 'FseV2,infinity,equal'
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 4 50 20 25 $out_dir 'rtp1start=0\&sctp1start=10000' 'FseV2,infinity,equal'
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 4 100 40 50 $out_dir 'rtp1start=0\&sctp1start=10000' 'FseV2,infinity,equal'
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 4 200 80 50 $out_dir 'rtp1start=0\&sctp1start=10000' 'FseV2,infinity,equal'

#bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 2 50 10 50 $out_dir 'rtp1start=0\&sctp1start=10000' 'FseV2,infinity,equal'

#bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 50 15 50 $out_dir 'rtp1start=0\&sctp1start=10000' 'FseV2,infinity,equal'
#bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 50 15 50 $out_dir 'sctp1start=0\&rtp1start=10000' 'FseV2,infinity,equal'

#bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 4 50 20 50 $out_dir 'rtp1start=0\&sctp1start=10000' 'FseV2,infinity,equal'
#bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 4 50 20 50 $out_dir 'sctp1start=0\&rtp1start=5000' 'FseV2,infinity,equal'

#bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 5 50 25 50 $out_dir 'rtp1start=0\&sctp1start=10000' 'FseV2,DesiredRateFseNgPaper,equal'
#bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 5 50 25 50 $out_dir 'rtp1start=0\&sctp1start=10000' 'FseV2,infinity,equal'
#bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 5 50 25 60 $out_dir 'sctp1start=0\&rtp1start=5000' 'FseV2,infinity,equal'

#bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 50 15 50 $out_dir 'rtp1start=0\&rtp2start=5000\&sctp1start=10000' 'FseV2,infinity,equal'
#bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 5 50 25 50 $out_dir 'rtp1start=0\&rtp2start=5000\&sctp1start=10000' 'FseV2,infinity,equal'

