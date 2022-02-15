out_dir=fse_ng_update_val_cases$(date --rfc-3339=date)

mkdir $out_dir

#### To show that updating with final_rate_only does not work
read -p 'Configure fse:fse_ng, fseNgVersion:extended, fseNgUpdateValue:final_rate_only, desiredRateCases:infinity, priorityCases:normal'

bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 5 50 26 75 $out_dir 'sctp1start=10000\&sctp1end=70000\&rtp1start=0\&rtp1end=60000' 'UpdateFinalRateOnly_good_case'

read -p 'Configure desiredRateCases:fse_ng_default'

bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 5 50 26 75 $out_dir 'sctp1start=10000\&sctp1end=70000\&rtp1start=0\&rtp1end=60000' 'UpdateFinalRateOnly_bad_case'

#Show improvement with delay only 
read -p 'Configure fseNgUpdateValue:delay_only, desiredRateCases:infinity'

bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 5 50 26 75 $out_dir 'sctp1start=10000\&sctp1end=70000\&rtp1start=0\&rtp1end=60000' 'Update_delay_based_DR_inf'

read -p 'Configure desiredRateCases:fse_ng_default'

bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 5 50 26 75 $out_dir 'sctp1start=10000\&sctp1end=70000\&rtp1start=0\&rtp1end=60000' 'Update_delay_based_DR_1.5mbit'
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 5 50 26 75 $out_dir 'sctp1start=10000\&sctp1end=70000\&rtp1start=0\&rtp1end=60000\&rtp2start=0\&rtp2end=40000' 'Update_delay_based_DR_1.5mbit'


