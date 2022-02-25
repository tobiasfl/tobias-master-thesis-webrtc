out_dir=fse_ng_long_run_cases$(date --rfc-3339=date)

mkdir $out_dir

bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 50 15 120 $out_dir 'sctp1start=0\&rtp1start=0' 'equal,FseNg,infinity'
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 50 15 120 $out_dir 'sctp1start=0\&rtp1start=0\&rtp2start=0' 'equal,FseNg,infinity'
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 5 50 25 120 $out_dir 'sctp1start=0\&rtp1start=0' 'equal,FseNg,infinity'
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 5 50 25 120 $out_dir 'sctp1start=0\&rtp1start=0' 'equal,FseNg,DesiredRateFseNgPaper'
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 5 50 25 120 $out_dir 'sctp1start=0\&rtp1start=0\&rtp2start=0' 'equal,FseNg,infinity'

