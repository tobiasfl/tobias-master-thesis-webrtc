out_dir=fse_ng_differing_priority_cases$(date --rfc-3339=date)

#### Show 3 flows with differing priorities

mkdir $out_dir

read -p 'Configure fse:fse_ng, fseNgVersion:extended, fseNgUpdateValue:delay_only, desiredRateCases:fse_ng_default, priorityCases:fse_ng_case2'

#Double priotiy for cwnd flows
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 50 15 75 $out_dir 'sctp1start=10000\&rtp1start=0\&rtp2start=5000' 'double_cwnd_priority'

read -p 'Configure priorityCases:fse_ng_case1'

#Double RTP priority
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 50 15 75 $out_dir 'sctp1start=10000\&rtp1start=0\&rtp2start=5000' 'double_rtp_priority'

read -p 'Configure priorityCases:all_diff'

#1RTP=1 SCTP=2 2RTP=3
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 50 15 75 $out_dir 'sctp1start=10000\&rtp1start=0\&rtp2start=5000' 'all_diff'

