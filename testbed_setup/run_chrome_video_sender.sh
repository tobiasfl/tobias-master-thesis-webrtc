
if [ $# -ne 2 ]; then
    echo "Usage: run_chromium.sh <test-app IP-address> <fake video file>"
    exit 1
fi
#Assumes google chrome is installed on the machine

google-chrome --use-fake-ui-for-media-stream --use-fake-device-for-media-stream --use-file-for-fake-video-capture=${2} --enable-logging=stderr --ignore-certificate-errors --log_level=0 --v=1 "https://${1}:8888?video" > log.txt 2>&1
