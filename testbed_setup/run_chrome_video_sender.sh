
if [ $# -eq 0 ]; then
    echo "Usage: run_chromium.sh <test-app IP-address>"
    exit 1
fi
#Assumes google chrome is installed on the machine

google-chrome --use-fake-ui-for-media-stream --use-fake-device-for-media-stream --use-file-for-fake-video-capture=$HOME/KristenAndSara_1280x720_60.y4m --enable-logging=stderr --ignore-certificate-errors --log_level=0 --v=1 "https://${1}:8888?video" > log.txt 2>&1
