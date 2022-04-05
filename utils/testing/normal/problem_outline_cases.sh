out_dir=normal_problem_outline_cases$(date --rfc-3339=date)

mkdir $out_dir

####NORMAL CHROME BROWSER CASES
## Single GCC reaching maximum throughput
#bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 50 50 247 120 $out_dir 'rtp1start=0' 'none' 'google-chrome'
#
## low bw case
#bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 5 50 25 120 $out_dir 'rtp1start=0\&sctp1start=10000' 'none' 'google-chrome'
#bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 5 50 25 120 $out_dir 'rtp1start=10000\&sctp1start=0' 'none' 'google-chrome'
#bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 5 50 25 120 $out_dir 'rtp1start=0\&sctp1start=0' 'none' 'google-chrome'
#
## higher bw case
#bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 50 50 247 120 $out_dir 'rtp1start=0\&sctp1start=10000' 'none' 'google-chrome'
#bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 50 50 247 120 $out_dir 'rtp1start=10000\&sctp1start=0' 'none' 'google-chrome'
#bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 50 50 247 120 $out_dir 'rtp1start=0\&sctp1start=0' 'none' 'google-chrome'
#
####CHROMIUM CODE CASES
## Single GCC reaching maximum throughput
#bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 50 50 247 120 $out_dir 'rtp1start=0' 'none' 
#
## low bw case
#bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 5 50 25 120 $out_dir 'rtp1start=0\&sctp1start=10000' 'none' 
#bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 5 50 25 120 $out_dir 'rtp1start=10000\&sctp1start=0' 'none' 
#bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 5 50 25 120 $out_dir 'rtp1start=0\&sctp1start=0' 'none' 
#
## higher bw case
#bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 50 50 247 120 $out_dir 'rtp1start=0\&sctp1start=10000' 'none' 
#bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 50 50 247 120 $out_dir 'rtp1start=10000\&sctp1start=0' 'none' 
#bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 50 50 247 120 $out_dir 'rtp1start=0\&sctp1start=0' 'none' 

### EXTRA
#bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 20 50 200 50 $out_dir 'rtp1start=0\&sctp1start=5000' 'none' 'google-chrome'
#bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 20 50 200 50 $out_dir 'rtp1start=5000\&sctp1start=0' 'none' 'google-chrome'
#bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 20 50 200 50 $out_dir 'rtp1start=0\&sctp1start=0' 'none' 'google-chrome'

bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 20 50 200 50 $out_dir 'sctp1start=10000' 'none' 'chromium'


