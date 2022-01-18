out/Default/chrome --use-fake-device-for-media-stream --use-file-for-fake-video-capture=KristenAndSara_1280x720_60.y4m --enable-logging=stderr --log_level=0 --v=3 --force-fieldtrials=WebRTC-Debugging-RtpDump/Enabled  https://localhost:8888 > log.txt 2>&1

#This enables SRTP packet dumping
#--enable-logging --v=3 --force-fieldtrials=WebRTC-Debugging-RtpDump/Enabled
