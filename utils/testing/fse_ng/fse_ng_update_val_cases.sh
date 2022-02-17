out_dir=fse_ng_update_val_cases$(date --rfc-3339=date)

mkdir $out_dir

#### To show that updating with final_rate_only does not work

bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 5 50 25 75 $out_dir 'sctp1start=10000\&sctp1end=70000\&rtp1start=0\&rtp1end=60000' 'none'

bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 5 50 25 75 $out_dir 'sctp1start=10000\&sctp1end=70000\&rtp1start=0\&rtp1end=60000' 'FseNg,DesiredRateFseNgPaper,equal,UpdateValFinalRate'
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 5 50 25 75 $out_dir 'sctp1start=10000\&sctp1end=70000\&rtp1start=0\&rtp1end=60000' 'FseNg,DesiredRateFseNgPaper,equal'



