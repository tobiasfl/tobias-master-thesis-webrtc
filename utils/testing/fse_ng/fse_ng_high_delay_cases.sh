out_dir=fse_ng_high_delay_cases$(date --rfc-3339=date)

#### To compare that it works alright at higher delay, but poorly on low delay cases

mkdir $out_dir

read -p 'Configure fse:fse_ng, fseNgVersion:extended, fseNgUpdateValue:delay_only, desiredRateCases:fse_ng_default, priorityCases:normal'

bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 50 16 75 $out_dir 'sctp1start=10000\&rtp1start=0' 'higher_delay'
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 50 16 75 $out_dir 'sctp1start=10000\&rtp1start=0\&rtp2start=5000' 'higher_delay'
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 50 16 75 $out_dir 'sctp1start=0\&rtp1start=10000\&rtp2start=15000' 'higher_delay'
