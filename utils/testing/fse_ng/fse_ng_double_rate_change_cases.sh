out_dir=fse_ng_double_rate_change_cases$(date --rfc-3339=date)

#### Show that they register double change of rate


mkdir $out_dir

read -p 'Configure fse:fse_ng, fseNgVersion:original, fseNgUpdateValue:delay_only, desiredRateCases:fse_ng_default, priorityCases:normal'

bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 50 16 50 $out_dir 'sctp1start=10000\&rtp1start=0' 'rate_change_original'

read -p 'Configure fseNgVersion:extended'

bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 50 16 50 $out_dir 'sctp1start=10000\&rtp1start=0' 'rate_change_extended'



