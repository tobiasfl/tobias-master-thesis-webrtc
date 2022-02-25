out_dir=fse_ng_rtt_problem_cases$(date --rfc-3339=date)

#### To show that it is problematic to only get the RTT from GCC

mkdir $out_dir

bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 50 15 40 $out_dir 'sctp1start=0\&rtp1start=0' 'equal,FseNg,infinity'
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 50 15 40 $out_dir 'sctp1start=0\&rtp1start=5000' 'equal,FseNg,infinity'
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 50 15 60 $out_dir 'sctp1start=0\&rtp1start=5000\&rtp2start=10000' 'equal,FseNg,infinity'
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 50 15 60 $out_dir 'sctp1start=0\&rtp1start=0\&rtp2start=0' 'equal,FseNg,infinity'
