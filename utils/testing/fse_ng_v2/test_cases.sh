out_dir=fse_ng_v2_test_cases$(date --rfc-3339=date)

mkdir $out_dir

# To make sure it works in normal cases
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 50 15 60 $out_dir 'rtp1start=0\&rtp2start=5000' 'FseNgV2,infinity,equal,'
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 50 15 60 $out_dir 'sctp1start=10000\&rtp1start=0' 'FseNgV2,infinity,equal,'
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 50 15 60 $out_dir 'sctp1start=10000\&rtp1start=0\&rtp2start=20000' 'FseNgV2,infinity,equal,'

# Starting at the same time problem
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 50 15 60 $out_dir 'sctp1start=0\&rtp1start=0' 'equal,FseNgV2,infinity'

#slow convergenve problem
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 6 50 30 60 $out_dir 'sctp1start=10000\&rtp1start=0' 'equal,FseNgV2,DesiredRateFseNgPaper'

#SCTP starting first problem
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 50 15 60 $out_dir 'sctp1start=0\&rtp1start=10000' 'equal,FseNgV2,infinity'
