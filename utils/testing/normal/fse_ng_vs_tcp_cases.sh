out_dir=normal_1rtp_vs_others$(date --rfc-3339=date)

mkdir $out_dir

read -p 'Configure  fseNgVersion:none'
read -p 'start iperf transfer'

bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 50 16 60 $out_dir 'rtp1start=0' '1rtp_vs_1tcp'

read -p 'stop iperf transfer'

bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 50 16 60 $out_dir 'rtp1start=0\&sctp1start=10000' '1rtp_vs_1sctp'
