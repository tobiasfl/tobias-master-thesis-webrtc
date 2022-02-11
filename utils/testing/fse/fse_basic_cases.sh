
out_dir=fse_basic_cases$(date --rfc-3339=date)

mkdir $out_dir

read -p 'Press enter when you have turned off fse'

bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 50 16 50 $out_dir 'rtp1start=0\&rtp1end=40000\&rtp2start=10000' 'withouth_fse'
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 50 16 50 $out_dir 'rtp1start=0\&rtp1end=40000\&rtp2start=5000\&sctp1start=10000' 'withouth_fse'

read -p 'Press enter when you have turned on fse, and set DR to 1gbit, with same priority'

#Both flows with DR infinity and same priority
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 50 16 50 $out_dir 'rtp1start=0\&rtp1end=40000\&rtp2start=10000' 'case1'

read -p 'Press enter when you have set DR of the first flow to 1mbit, with same priority'

#First flow having DR=1mbit and both having same priority
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 50 16 50 $out_dir 'rtp1start=0\&rtp1end=40000\&rtp2start=10000' 'case2'

read -p 'Press enter when you have set DR of both flows to 1gbit, first flow having priority 1 and second having priority 2'

#Both flows with DR infinity but one having double priority
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 50 16 50 $out_dir 'rtp1start=0\&rtp1end=40000\&rtp2start=10000' 'case3'
