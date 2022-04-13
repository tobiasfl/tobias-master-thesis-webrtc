out_dir=debugging$(date --rfc-3339=date)

mkdir $out_dir

bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 50 15 60 $out_dir 'rtp1start=0' 'FseNgV2,infinity,equal,'
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 50 15 60 $out_dir 'rtp1start=0\&rtp2start=5000' 'FseNgV2,infinity,equal,'
