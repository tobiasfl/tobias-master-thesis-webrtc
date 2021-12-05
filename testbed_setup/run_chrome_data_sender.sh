if [ $# -eq 0 ]; then
    echo ": run_chromium.sh <test-app IP-address>"
    exit 1
fi
#Assumes google chrome is installed on the machine

google-chrome --use-fake-ui-for-media-stream --enable-logging=stderr --log_level=0 --v=1 "https://${1}:8888?data" > log.txt 2>&1
