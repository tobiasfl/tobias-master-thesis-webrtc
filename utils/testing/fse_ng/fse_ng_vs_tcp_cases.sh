out_dir=fse_ng_vs_1tcp_cases$(date --rfc-3339=date)

mkdir $out_dir

#TODO: run_test should be able to start and stop a tcp transfer in the background 
#read -p 'Configure fse:fse_ng, fseNgVersion:extended, fseNgUpdateValue:delay_only, desiredRateCases:fse_ng_default, priorityCases:equal'
#read -p 'start iperf transfer and ss script'
#
#bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 50 16 75 $out_dir 'sctp1start=10000\&rtp1start=0' 'fse_ng'

#read -p 'stop iperf transfer and ss script'
read -p 'Configure fse:fse_ng, fseNgVersion:none, fseNgUpdateValue:delay_only, desiredRateCases:fse_ng_default, priorityCases:equal'
read -p 'start iperf transfer and ss script'

bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 50 16 75 $out_dir 'sctp1start=10000\&rtp1start=0' 'without_fse_ng'
