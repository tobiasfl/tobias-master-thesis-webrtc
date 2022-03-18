out_dir=fse_v2_basic_cases$(date --rfc-3339=date)

mkdir $out_dir

#Both flows with DR infinity and same priority
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 50 15 35 $out_dir 'rtp1start=0\&rtp1end=35000\&rtp2start=5000' 'FseV2,infinity,equal'
                                                                                                 
#First flow having DR=1mbit and both having same priority                                        
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 50 15 35 $out_dir 'rtp1start=0\&rtp1end=35000\&rtp2start=5000' 'FseV2,DesiredRateFseCase2,equal'

#Both flows having DR=1mbit and both having same priority                                        
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 50 15 35 $out_dir 'rtp1start=0\&rtp1end=35000\&rtp2start=5000' 'FseV2,DesiredRateAllLimited,equal'

#Both flows with DR infinity but one having double priority                                      
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 50 15 35 $out_dir 'rtp1start=0\&rtp1end=35000\&rtp2start=5000' 'FseV2,infinity,PriorityFseCase3'
                                                                                                 
#See what happens with SCTP running as well                                                      
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 50 15 35 $out_dir 'rtp1start=0\&sctp1start=5000' 'FseV2,infinity,equal'
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 50 15 35 $out_dir 'rtp1start=0\&rtp2start=5000\&sctp1start=10000' 'FseV2,infinity,equal'

# SCTP starting first
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 50 15 35 $out_dir 'sctp1start=0\&rtp1start=5000' 'FseV2,infinity,equal'
bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 50 15 35 $out_dir 'sctp1start=0\&rtp1start=5000\&rtp2start=10000' 'FseV2,infinity,equal'
