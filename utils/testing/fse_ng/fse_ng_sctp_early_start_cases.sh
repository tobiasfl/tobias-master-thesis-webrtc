out_dir=fse_ng_sctp_early_start_cases$(date --rfc-3339=date)

mkdir $out_dir

bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 50 15 60 $out_dir 'sctp1start=0\&rtp1start=15000' 'equal,FseNg,infinity'
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 50 15 60 $out_dir 'sctp1start=0\&rtp1start=10000\&rtp2start=15000' 'equal,FseNg,infinity'
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 5 50 25 60 $out_dir 'sctp1start=0\&rtp1start=15000' 'equal,FseNg,infinity'
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 5 50 25 60 $out_dir 'sctp1start=0\&rtp1start=15000' 'equal,FseNg,DesiredRateFseNgPaper'
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 5 50 25 60 $out_dir 'sctp1start=0\&rtp1start=10000\&rtp2start=15000' 'equal,FseNg,infinity'

