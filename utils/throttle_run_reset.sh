# Throttle LO interface 
# TODO: find appropriate latency and burst values
sudo tc qdisc add dev enp0s31f6 root tbf rate 2mbit latency 50ms burst 50kb

out/Debug/chrome --use-fake-device-for-media-stream --use-file-for-fake-video-capture=KristenAndSara_1280x720_60.y4m --enable-logging --v=3 --force-fieldtrials=WebRTC-Debugging-RtpDump/Enabled > log.txt 2>&1

# Go back to the way things were.
sudo tc qdisc del dev enp0s31f6 root
