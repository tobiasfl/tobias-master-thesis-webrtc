out_dir=single_sctp$(date --rfc-3339=date)

mkdir $out_dir

bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 2 100 20 60 $out_dir 'sctp1start=0' 'none,CoupleDcSctp'
