out_dir=fse_v2_differing_bw_cases$(date --rfc-3339=date)


mkdir $out_dir

# 4mbit
# RTP starting first
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 4 100 40 60 $out_dir 'rtp1start=0\&sctp1start=5000' 'FseV2,DesiredRateFseNgPaper,equal'
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 4 100 40 60 $out_dir 'rtp1start=0\&rtp2start=5000\&sctp1start=10000' 'FseV2,DesiredRateFseNgPaper,equal'
                                                                  
# SCTP starting first
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 4 100 40 60 $out_dir 'sctp1start=0\&rtp1start=10000' 'FseV2,DesiredRateFseNgPaper,equal'

# 5mbit
# RTP starting first
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 5 100 56 60 $out_dir 'rtp1start=0\&sctp1start=5000' 'FseV2,DesiredRateFseNgPaper,equal'

bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 5 100 56 60 $out_dir 'rtp1start=0\&rtp2start=5000\&sctp1start=10000' 'FseV2,DesiredRateFseNgPaper,equal'

# SCTP starting first
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 5 100 56 60 $out_dir 'sctp1start=0\&rtp1start=10000' 'FseV2,DesiredRateFseNgPaper,equal'

# 6mbit
# RTP starting first
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 6 100 60 60 $out_dir 'rtp1start=0\&sctp1start=5000' 'FseV2,DesiredRateFseNgPaper,equal'
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 6 100 60 60 $out_dir 'rtp1start=0\&rtp2start=5000\&sctp1start=10000' 'FseV2,DesiredRateFseNgPaper,equal'
                                                                   
# SCTP starting first
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 6 100 60 60 $out_dir 'sctp1start=0\&rtp1start=10000' 'FseV2,DesiredRateFseNgPaper,equal'

# 7.5mbit
# RTP starting first
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 7.5 100 84 60 $out_dir 'rtp1start=0\&sctp1start=5000' 'FseV2,DesiredRateFseNgPaper,equal'
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 7.5 100 84 60 $out_dir 'rtp1start=0\&rtp2start=5000\&sctp1start=10000' 'FseV2,DesiredRateFseNgPaper,equal'
                                                                   
# SCTP starting first
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 7.5 100 84 60 $out_dir 'sctp1start=0\&rtp1start=10000' 'FseV2,DesiredRateFseNgPaper,equal'


