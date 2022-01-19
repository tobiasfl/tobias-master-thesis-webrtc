cat log.txt | grep -a SCTP_PACKET > filtered.log

text2pcap -t "%H:%M:%S." -D -u 1024,1024 filtered.log filtered.pcap


# The value "1024" isn't important, we just need a port for the dummy UDP
# headers generated. Lastly, you should be able to open filtered.pcap in
# Wireshark, then right click a packet and "Decode As..." SCTP.
#
# Why do all this? Because SCTP goes over DTLS, which is encrypted. So just
# getting a normal packet capture won't help you, unless you have the DTLS
# keying material.
