out_dir=gcc_mismatch$(date --rfc-3339=date)

mkdir $out_dir

###CHROMIUM CODE CASES

# using link bandwidth only
#bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 0.5 100 6 120 $out_dir 'rtp1start=0' 'none' 
#bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 1 100 11 120 $out_dir 'rtp1start=10000' 'none' 
#bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 1.5 100 16 120 $out_dir 'rtp1start=0' 'none' 
#bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 2 100 21 120 $out_dir 'rtp1start=0' 'none' 
#bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 2.5 100 27.0 120 $out_dir 'rtp1start=0' 'none' 


# setting to constant rates in code 
for BW in "0.5" "1" "1.5" "2" "2.5" "3"
do  
    read -p "set constant rate of ${BW}mbps and compile"
    for RTT in "50" "100" "200" 
    do
        BDP=$(echo "(($RTT*5)/(8*1.1267))+1" | bc -q)
        echo $BDP
        const_rates_out_dir="$out_dir/const_rate_${BW}_${RTT}"
        echo $const_rates_out_dir
        mkdir $const_rates_out_dir
        bash ~/Code/tobias-master-thesis-webrtc/utils/testing/run_test.sh 5 $RTT $BDP 90 $const_rates_out_dir 'rtp1start=0' 'none' 
    done
done

