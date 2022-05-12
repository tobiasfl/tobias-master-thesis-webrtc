out_dir=normal_problem_outline_cases$(date --rfc-3339=date)

mkdir $out_dir

#####NORMAL CHROME BROWSER CASES
## Single GCC reaching maximum throughput
#bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 10 50 50 120 $out_dir 'rtp1start=0' 'none' 'chromium'

# low bw case
#bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 5 50 25 120 $out_dir 'rtp1start=0\&sctp1start=10000' 'none' 'chromium'
#bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 5 50 25 120 $out_dir 'rtp1start=10000\&sctp1start=0' 'none' 'chromium'
#bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 5 50 25 120 $out_dir 'rtp1start=0\&sctp1start=0' 'none' 'chromium'
#
## higher bw case
#bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 10 50 50 120 $out_dir 'rtp1start=0\&sctp1start=10000' 'none' 'chromium'
#bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 10 50 50 120 $out_dir 'rtp1start=10000\&sctp1start=0' 'none' 'chromium'
#bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 10 50 50 120 $out_dir 'rtp1start=0\&sctp1start=0' 'none' 'chromium'
#
####CHROMIUM CODE CASES
## Single GCC reaching maximum throughput
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 10 100 99 120 $out_dir 'rtp1start=0' 'none' 
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 10 100 99 120 $out_dir 'sctp1start=0' 'none' 
#
## low bw case
#bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 5 50 25 120 $out_dir 'rtp1start=0\&sctp1start=10000' 'none' 
#bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 5 50 25 120 $out_dir 'rtp1start=10000\&sctp1start=0' 'none' 
#bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 5 50 25 120 $out_dir 'rtp1start=0\&sctp1start=0' 'none' 
#
## higher bw case
#bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 10 50 50 120 $out_dir 'rtp1start=0\&sctp1start=10000' 'none' 
#bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 10 50 50 120 $out_dir 'rtp1start=10000\&sctp1start=0' 'none' 
#bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 10 50 50 120 $out_dir 'rtp1start=0\&sctp1start=0' 'none' 

