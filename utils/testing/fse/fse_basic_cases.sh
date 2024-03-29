out_dir=fse_basic_cases$(date --rfc-3339=date)

mkdir $out_dir



bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_chromium.sh $out_dir 'https://localhost:8888?rtp1start=0\&rtp1end=65000' 'Fse,infinity,equal'
#bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 50 15 75 $out_dir 'rtp1start=0\&rtp1end=65000\&rtp2start=5000' 'none'
#bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 50 15 75 $out_dir 'rtp1start=0\&rtp1end=65000\&rtp2start=5000\&sctp1start=10000' 'none'
#
##Both flows with DR infinity and same priority
#bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 50 15 75 $out_dir 'rtp1start=0\&rtp1end=65000\&rtp2start=5000' 'Fse,infinity,equal'
#                                                                                                   
##First flow having DR=1mbit and both having same priority                                          
#bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 50 15 75 $out_dir 'rtp1start=0\&rtp1end=65000\&rtp2start=5000' 'Fse,DesiredRateFseCase2,equal'
#                                                                                                   
##Both flows with DR infinity but one having double priority                                        
#bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 50 15 75 $out_dir 'rtp1start=0\&rtp1end=65000\&rtp2start=5000' 'Fse,infinity,PriorityFseCase3'
#                                                                                                   
##See what happens with SCTP running as well                                                        
#bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 3 50 15 75 $out_dir 'rtp1start=0\&rtp1end=65000\&rtp2start=5000\&sctp1start=10000' 'Fse,infinity,equal'
