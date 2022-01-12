if [ $# -eq 0 ]; then
    echo "Usage: run_chromium.sh <output_file_name>"
    exit 1
fi

sudo tcpdump -i enp0s31f6 -w "${1}_tcpdump.pcap" &
TCPDUMP_PID=$!

out/Debug/chrome --use-fake-device-for-media-stream --use-file-for-fake-video-capture=KristenAndSara_1280x720_60.y4m --enable-logging=stderr --v=1 https://localhost:8888  > log.txt 2>&1

kill -INT $TCPDUMP_PID

python3 "rateplotter.py" "log.txt" "$1"
