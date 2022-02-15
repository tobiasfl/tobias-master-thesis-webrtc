
out_dir=fse_ng_low_delay_cases$(date --rfc-3339=date)

#### Show that it still is not perfect with lower delay because it takes long time for SCTP to fill link

mkdir $out_dir

read -p 'Configure fse:fse_ng, fseNgVersion:extended, fseNgUpdateValue:delay_only, desiredRateCases:fse_ng_default, priorityCases:normal'

bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 10 4 75 $out_dir 'sctp1start=10000\&rtp1start=0' 'lower_delay'
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 10 4 75 $out_dir 'sctp1start=10000\&rtp1start=0\&rtp2start=5000' 'lower_delay'
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 10 4 75 $out_dir 'sctp1start=0\&rtp1start=10000\&rtp2start=15000' 'lower_delay'
