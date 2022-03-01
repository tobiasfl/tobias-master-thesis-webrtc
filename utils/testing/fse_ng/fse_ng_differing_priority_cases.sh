out_dir=fse_ng_differing_priority_cases$(date --rfc-3339=date)

#### Show 3 flows with differing priorities

mkdir $out_dir

#1RTP=1 2RTP=2
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 50 15 75 $out_dir 'rtp1start=0\&rtp2start=5000' 'FseNg,infinity,PriorityVaried'

#Double priotiy for cwnd flows
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 50 15 75 $out_dir 'sctp1start=10000\&rtp1start=0\&rtp2start=5000' 'FseNg,infinity,PriorityCwndFlowDouble'

#Double RTP priority
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 50 15 75 $out_dir 'sctp1start=10000\&rtp1start=0\&rtp2start=5000' 'FseNg,infinity,PriorityRateFlowDouble'

#1RTP=1 SCTP=2 2RTP=3
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 50 15 75 $out_dir 'sctp1start=10000\&rtp1start=0\&rtp2start=5000' 'FseNg,infinity,PriorityVaried'

