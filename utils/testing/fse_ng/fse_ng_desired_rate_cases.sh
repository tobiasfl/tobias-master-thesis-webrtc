
out_dir=fse_ng_desired_rate_cases$(date --rfc-3339=date)

mkdir $out_dir

read -p 'Press any key when you have configured desired rate to be 1.5mbit and priorities to be normal'

bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 5 50 26 55 $out_dir 'sctp1start=15000\&sctp1end=50000\&rtp1start=0\&rtp1end=40000' 'normal_prios_DR_1.5'
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 5 50 26 55 $out_dir 'sctp1start=0\&sctp1end=40000\&rtp1start=15000\&rtp1end=50000' 'normal_prios_DR_1.5'

bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 7.5 50 39 55 $out_dir 'sctp1start=15000\&sctp1end=50000\&rtp1start=0\&rtp1end=40000' 'normal_prios_DR_1.5'
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 7.5 50 39 55 $out_dir 'sctp1start=0\&sctp1end=40000\&rtp1start=15000\&rtp1end=50000' 'normal_prios_DR_1.5'

bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 10 50 51 55 $out_dir 'sctp1start=15000\&sctp1end=50000\&rtp1start=0\&rtp1end=40000' 'normal_prios_DR_1.5'
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 10 50 51 55 $out_dir 'sctp1start=0\&sctp1end=40000\&rtp1start=15000\&rtp1end=50000' 'normal_prios_DR_1.5'
