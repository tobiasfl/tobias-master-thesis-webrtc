bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 10 10 11 60 . 'sctp1start=0'
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 10 10 11 60 . 'sctp1start=0\&rtp1start=20000'
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 10 10 11 60 . 'sctp1start=20000\&rtp1start=0'
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 10 10 11 60 . 'sctp1start=20000\&rtp1start=0\&rtp2start=40000'
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 10 10 11 60 . 'sctp1start=0\&rtp1start=20000\&rtp2start=40000'

bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 10 50 51 60 . 'sctp1start=0'
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 10 50 51 60 . 'sctp1start=0\&rtp1start=20000'
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 10 50 51 60 . 'sctp1start=20000\&rtp1start=0'
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 10 50 51 60 . 'sctp1start=20000\&rtp1start=0\&rtp2start=40000'
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 10 50 51 60 . 'sctp1start=0\&rtp1start=20000\&rtp2start=40000'

