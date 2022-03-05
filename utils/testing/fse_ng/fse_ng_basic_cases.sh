out_dir=fse_ng_basic_cases$(date --rfc-3339=date)

mkdir $out_dir

#Simply two rtp flows
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 50 15 75 $out_dir 'rtp1start=0\&rtp1end=65000\&rtp2start=5000' 'FseNg,infinity,equal,'
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 50 15 75 $out_dir 'rtp1start=0\&rtp1end=65000\&rtp2start=5000' 'none,infinity,equal,'

#Two flows, different starting times
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 50 15 65 $out_dir 'sctp1start=10000\&sctp1end=60000\&rtp1start=0\&rtp1end=50000' 'FseNg,infinity,equal,'
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 50 15 65 $out_dir 'sctp1start=10000\&sctp1end=60000\&rtp1start=0\&rtp1end=50000' 'none,infinity,equal,'

bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 50 15 65 $out_dir 'sctp1start=0\&sctp1end=50000\&rtp1start=10000\&rtp1end=60000' 'FseNg,infinity,equal,'
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 50 15 65 $out_dir 'sctp1start=0\&sctp1end=50000\&rtp1start=10000\&rtp1end=60000' 'none,infinity,equal,'

#Three flows
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 50 15 120 $out_dir 'sctp1start=10000\&rtp1start=0\&rtp1end=100000\&rtp2start=20000\&rtp2end=110000' 'FseNg,infinity,equal,'
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 50 15 120 $out_dir 'sctp1start=10000\&rtp1start=0\&rtp1end=100000\&rtp2start=20000\&rtp2end=110000' 'none,infinity,equal,'

