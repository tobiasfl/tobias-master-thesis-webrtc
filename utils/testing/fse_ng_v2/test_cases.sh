out_dir=fse_ng_v2_test_cases$(date --rfc-3339=date)

mkdir $out_dir

# To make sure it works in normal cases
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 50 15 60 $out_dir 'rtp1start=0\&rtp2start=5000' 'FseNgV2,infinity,equal,'
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 50 15 60 $out_dir 'rtp1start=0\&rtp2start=5000' 'FseNg,infinity,equal,'
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 50 15 60 $out_dir 'sctp1start=10000\&rtp1start=0' 'FseNgV2,infinity,equal,'
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 50 15 60 $out_dir 'sctp1start=10000\&rtp1start=0' 'FseNg,infinity,equal,'
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 50 15 60 $out_dir 'sctp1start=10000\&rtp1start=0\&rtp2start=20000' 'FseNgV2,infinity,equal,'
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 50 15 60 $out_dir 'sctp1start=10000\&rtp1start=0\&rtp2start=20000' 'FseNg,infinity,equal,'

# Starting at the same time problem
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 50 15 60 $out_dir 'sctp1start=0\&rtp1start=0' 'equal,FseNgV2,infinity'
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 50 15 60 $out_dir 'sctp1start=0\&rtp1start=0' 'equal,FseNg,infinity'

#slow convergence problem
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 6 50 30 60 $out_dir 'sctp1start=10000\&rtp1start=0' 'equal,FseNgV2,DesiredRateFseNgPaper'
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 6 50 30 60 $out_dir 'sctp1start=10000\&rtp1start=0' 'equal,FseNG,DesiredRateFseNgPaper'

#SCTP starting first problem
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 50 15 60 $out_dir 'sctp1start=0\&rtp1start=10000' 'equal,FseNgV2,infinity'
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 50 15 60 $out_dir 'sctp1start=0\&rtp1start=10000' 'equal,FseNg,infinity'

#1RTP=1 2RTP=2
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 50 15 60 $out_dir 'rtp1start=0\&rtp2start=5000' 'FseNgV2,infinity,PriorityVaried'
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 50 15 60 $out_dir 'rtp1start=0\&rtp2start=5000' 'FseNg,infinity,PriorityVaried'

#Double priotiy for cwnd flows
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 50 15 60 $out_dir 'sctp1start=10000\&rtp1start=0\&rtp2start=5000' 'FseNgV2,infinity,PriorityCwndFlowDouble'
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 50 15 60 $out_dir 'sctp1start=10000\&rtp1start=0\&rtp2start=5000' 'FseNg,infinity,PriorityCwndFlowDouble'

#Double RTP priority
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 50 15 60 $out_dir 'sctp1start=10000\&rtp1start=0\&rtp2start=5000' 'FseNgV2,infinity,PriorityRateFlowDouble'
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 50 15 60 $out_dir 'sctp1start=10000\&rtp1start=0\&rtp2start=5000' 'FseNg,infinity,PriorityRateFlowDouble'

#1RTP=1 SCTP=2 2RTP=3
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 50 15 60 $out_dir 'sctp1start=10000\&rtp1start=0\&rtp2start=5000' 'FseNgV2,infinity,PriorityVaried'
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 50 15 60 $out_dir 'sctp1start=10000\&rtp1start=0\&rtp2start=5000' 'FseNg,infinity,PriorityVaried'

