cat log.txt | grep -a RTP_DUMP > rtp_dump.log

text2pcap -t "%H:%M:%S." -D -u 1000,2000 rtp_dump.log rtp_dump.pcap

