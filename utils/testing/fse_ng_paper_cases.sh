
out_dir=fse_ng_paper_cases$(date --rfc-3339=date)

mkdir $out_dir

read -p 'Press any key when you have configured desired rate to be 1.5mbit'

bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3.5 50 100 70 $out_dir 'sctp1start=15000\&sctp1end=70000\&rtp1start=0\&rtp1end=60000\&rtp2start=0\&rtp2end=40000'
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 10 50 100 70 $out_dir 'sctp1start=15000\&sctp1end=70000\&rtp1start=0\&rtp1end=60000\&rtp2start=0\&rtp2end=40000'

read -p 'Press any key when you have configured priority case to be fse_ng_case1'
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3.5 50 100 70 $out_dir 'sctp1start=15000\&sctp1end=70000\&rtp1start=0\&rtp1end=60000\&rtp2start=0\&rtp2end=40000'

read -p 'Press any key when you have configured priority case to be fse_ng_case2'
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3.5 50 100 70 $out_dir 'sctp1start=15000\&sctp1end=70000\&rtp1start=0\&rtp1end=60000\&rtp2start=0\&rtp2end=40000'


