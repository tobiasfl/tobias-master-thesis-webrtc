
## Chromium

The cwnd is updated by the functions in netinet/sctp_cc_functions.c. See
https://cs.chromium.org/chromium/src/third_party/usrsctp/usrsctplib/usrsctplib/netinet/sctp_cc_functions.c?g=0
You just need to provide some callbacks to implement another CC algorithm. This CC
algorithm can then be selected by a socket option.
 

### WebRTC Test application
https://github.com/tobiasfl/webrtc-test-app
