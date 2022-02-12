
out_dir=fse_ng_basic_cases$(date --rfc-3339=date)

mkdir $out_dir

read -p 'Press any key when you have configured desired rate to be 10mbit and priorities to be normal'

bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 5 50 26 55 $out_dir 'sctp1start=0\&sctp1end=30000' 'normal_prios_DR_inf'

bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 5 50 26 55 $out_dir 'sctp1start=15000\&sctp1end=50000\&rtp1start=0\&rtp1end=40000' 'normal_prios_DR_inf'
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 5 50 26 55 $out_dir 'sctp1start=0\&sctp1end=40000\&rtp1start=15000\&rtp1end=50000' 'normal_prios_DR_inf'

bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 7.5 50 39 55 $out_dir 'sctp1start=0\&sctp1end=30000' 'normal_prios_DR_inf'

bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 7.5 50 39 55 $out_dir 'sctp1start=15000\&sctp1end=50000\&rtp1start=0\&rtp1end=40000' 'normal_prios_DR_inf'
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 7.5 50 39 55 $out_dir 'sctp1start=0\&sctp1end=40000\&rtp1start=15000\&rtp1end=50000' 'normal_prios_DR_inf'

bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 10 50 51 55 $out_dir 'sctp1start=0\&sctp1end=30000' 'normal_prios_DR_inf'

bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 10 50 51 55 $out_dir 'sctp1start=15000\&sctp1end=50000\&rtp1start=0\&rtp1end=40000' 'normal_prios_DR_inf'
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 10 50 51 55 $out_dir 'sctp1start=0\&sctp1end=40000\&rtp1start=15000\&rtp1end=50000' 'normal_prios_DR_inf'
