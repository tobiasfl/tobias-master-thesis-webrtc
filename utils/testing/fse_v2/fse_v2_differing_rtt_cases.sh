
out_dir=fse_v2_differing_rtt_cases$(date --rfc-3339=date)


mkdir $out_dir

# 10ms
# RTP starting first
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 10 4 85 $out_dir 'rtp1start=0\&sctp1start=10000' 'FseV2,infinity,equal'
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 10 4 85 $out_dir 'rtp1start=0\&rtp2start=10000\&sctp1start=20000\&rtp1end=65000\&rtp2end=75000' 'FseV2,infinity,equal'

# SCTP starting first
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 10 4 85 $out_dir 'sctp1start=0\&rtp1start=10000\&sctp1end=75000' 'FseV2,infinity,equal'
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 10 4 85 $out_dir 'sctp1start=0\&rtp1start=10000\&rtp2start=20000\&rtp1end=65000\&rtp2end=75000' 'FseV2,infinity,equal'

# 25ms
# RTP starting first
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 25 9 85 $out_dir 'rtp1start=0\&sctp1start=10000' 'FseV2,infinity,equal'
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 25 9 85 $out_dir 'rtp1start=0\&rtp2start=10000\&sctp1start=20000\&rtp1end=65000\&rtp2end=75000' 'FseV2,infinity,equal'

# SCTP starting first
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 25 9 85 $out_dir 'sctp1start=0\&rtp1start=10000\&sctp1end=75000' 'FseV2,infinity,equal'
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 25 9 85 $out_dir 'sctp1start=0\&rtp1start=10000\&rtp2start=20000\&rtp1end=65000\&rtp2end=75000' 'FseV2,infinity,equal'

# 100ms
# RTP starting first
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 100 31 85 $out_dir 'rtp1start=0\&sctp1start=10000' 'FseV2,infinity,equal'
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 100 31 85 $out_dir 'rtp1start=0\&rtp2start=10000\&sctp1start=20000\&rtp1end=65000\&rtp2end=75000' 'FseV2,infinity,equal'

# SCTP starting first
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 100 31 85 $out_dir 'sctp1start=0\&rtp1start=10000\&sctp1end=75000' 'FseV2,infinity,equal'
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 100 31 85 $out_dir 'sctp1start=0\&rtp1start=10000\&rtp2start=20000\&rtp1end=65000\&rtp2end=75000' 'FseV2,infinity,equal'

# 200ms
# RTP starting first
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 200 67 85 $out_dir 'rtp1start=0\&sctp1start=10000' 'FseV2,infinity,equal'
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 200 67 85 $out_dir 'rtp1start=0\&rtp2start=10000\&sctp1start=20000\&rtp1end=65000\&rtp2end=75000' 'FseV2,infinity,equal'
                                                                        
# SCTP starting first                                                   
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 200 67 85 $out_dir 'sctp1start=0\&rtp1start=10000\&sctp1end=75000' 'FseV2,infinity,equal'
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 200 67 85 $out_dir 'sctp1start=0\&rtp1start=10000\&rtp2start=20000\&rtp1end=65000\&rtp2end=75000' 'FseV2,infinity,equal'

