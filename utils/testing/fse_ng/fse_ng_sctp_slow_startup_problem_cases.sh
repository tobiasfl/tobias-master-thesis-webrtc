out_dir=fse_ng_sctp_slow_startup_problem_cases$(date --rfc-3339=date)

mkdir $out_dir

#bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 5 50 25 60 $out_dir 'sctp1start=10000\&rtp1start=0' 'equal,FseNg,DesiredRateFseNgPaper'
#bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 5 50 25 60 $out_dir 'sctp1start=10000\&rtp1start=0' 'equal,FseNg,infinity'
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 6 50 30 60 $out_dir 'sctp1start=10000\&rtp1start=0' 'equal,FseNg,DesiredRateFseNgPaper'
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 6 50 30 60 $out_dir 'sctp1start=10000\&rtp1start=0' 'equal,FseNg,infinity'
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 7.5 50 37 90 $out_dir 'sctp1start=10000\&rtp1start=0' 'equal,FseNg,DesiredRateFseNgPaper'
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 7.5 50 37 90 $out_dir 'sctp1start=10000\&rtp1start=0' 'equal,FseNg,infinity'
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 10 50 50 110 $out_dir 'sctp1start=10000\&rtp1start=0' 'equal,FseNg,DesiredRateFseNgPaper'
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 10 50 50 110 $out_dir 'sctp1start=10000\&rtp1start=0' 'equal,FseNg,infinity'

