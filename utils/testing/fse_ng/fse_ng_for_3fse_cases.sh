out_dir=fse_ng_for_3fse_cases$(date --rfc-3339=date)

mkdir $out_dir

##### Show it works for RTP flows only in FSE- cases 1,2,3

read -p 'Configure fse:fse_ng, fseNgVersion:extended, fseNgUpdateValue:delay_only, desiredRateCases:infinity, priorityCases:normal'

bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 50 16 50 $out_dir 'rtp1start=0\&rtp1end=40000\&rtp2start=5000\&rtp2end=50000' 'fse_case1'

read -p 'Configure desiredRateCases:fse_case2'

bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 50 16 50 $out_dir 'rtp1start=0\&rtp1end=40000\&rtp2start=5000\&rtp2end=50000' 'fse_case2'

read -p 'Configure Prioritcase:fse_case3, desiredRateCases:infinity'

bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 50 16 50 $out_dir 'rtp1start=0\&rtp1end=40000\&rtp2start=5000\&rtp2end=50000' 'fse_case3'

