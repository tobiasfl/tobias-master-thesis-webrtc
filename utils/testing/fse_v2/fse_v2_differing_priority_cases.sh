out_dir=fse_v2_differing_priority_cases$(date --rfc-3339=date)

#### Show 3 flows with differing priorities

mkdir $out_dir

#1RTP=1 2RTP=2
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 50 15 50 $out_dir 'rtp1start=0\&rtp2start=5000' 'FseV2,infinity,PriorityVaried'

#Double priotiy for cwnd flows
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 50 15 50 $out_dir 'sctp1start=10000\&rtp1start=0\&rtp2start=5000' 'FseV2,infinity,PriorityCwndFlowDouble'

#Double RTP priority
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 50 15 50 $out_dir 'sctp1start=10000\&rtp1start=0\&rtp2start=5000' 'FseV2,infinity,PriorityRateFlowDouble'

#1RTP=1 SCTP=2 2RTP=3
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 50 15 50 $out_dir 'sctp1start=10000\&rtp1start=0\&rtp2start=5000' 'FseV2,infinity,PriorityVaried'

