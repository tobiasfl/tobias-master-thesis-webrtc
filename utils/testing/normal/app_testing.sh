out_dir=app_testing$(date --rfc-3339=date)

mkdir $out_dir


bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 2 100 15 40 $out_dir 'rtp1start=0\&rtp2start=5000\&rtp1end=15000\&rtp2end=20000\&sctp1start=0\&sctp1end=20000' 'none' 
