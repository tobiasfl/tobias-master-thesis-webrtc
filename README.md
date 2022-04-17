## Overleaf link
https://www.overleaf.com/project/60817333215ca555bef3aa17

## FSE-NG test cases notes
https://docs.google.com/document/d/1Lhr-acZPmxIhwwvKZJcPSE-YcYC3q1x4ztqg2QlQho4/edit?usp=sharing

## Background

The following papers are important for implementing <a href="http://heim.ifi.uio.no/davihay/hayes14__pract_passiv_shared_bottl_detec-abstract.html">shared bottlenect detection</a> and <a href="http://heim.ifi.uio.no/safiquli/coupled-cc/"> coupled congestion control </a> in <a href="https://www.chromium.org/Home">chromium</a> for webrtc.


### Google Congestion Control
<ol>
        <li> <a href="https://tools.ietf.org/html/draft-ietf-rmcat-gcc-02"> Draft </a></li>
        <li> <a href="http://conferences.sigcomm.org/sigcomm/2013/papers/fhmn/p21.pdf">Paper 1 </a> - this is a good one to get the background, however, the evaluation is based on the older version of GCC</li>
        <li> <a href="http://c3lab.poliba.it/images/c/ce/Gcc-pv-2013.pdf"> [Paper 2] </a> and <a href="http://c3lab.poliba.it/images/3/3d/Elastic-slides.pdf"> [Slides] </a>  written by the same author of paper 1->.</li>
        <li> <a href="http://www.netlab.tkk.fi/~varun/singh2013rrtcc.pdf"> Paper 3 </a>. Also provides evaluation for the older version.
        I recommend paper 1.</li>
</ol>


### Shared Bottleneck Detection
<ol>
    <li> <a href="https://tools.ietf.org/html/rfc8382"> RFC8382 </a>  </li>
    <li> <a href="https://ieeexplore.ieee.org/document/6925767"> Paper </a> </li>
    <li> <a href="https://ieeexplore.ieee.org/document/9161279"> paper 2 </a> </li> 
</ol>


### Coupled Congestion Control
<ol>
        <li> <a href="https://tools.ietf.org/html/rfc8699"> RFC8699 </a> </li>
        <li> <a href="http://dl.acm.org/authorize.cfm?key=N71345"> Paper-1 </a> </li>
        <li> <a href="https://ieeexplore.ieee.org/document/7502803"> Paper-1 </a> </li>
</ol>

## Chromium

The cwnd is updated by the functions in netinet/sctp_cc_functions.c. See
https://cs.chromium.org/chromium/src/third_party/usrsctp/usrsctplib/usrsctplib/netinet/sctp_cc_functions.c?g=0
You just need to provide some callbacks to implement another CC algorithm. This CC
algorithm can then be selected by a socket option.

Video Channel: (see bitrate controller for sender-side estimation:)

https://cs.chromium.org/chromium/src/third_party/webrtc/modules/


If you want to test in a local testbed and use a stored video file for camera input, you need to start chrome with:

	./out/Debug/chrome --use-fake-device-for-media-stream --use-file-for-fake-video-capture=path/to/file.y4m

You can easily convert yuv, webm, etc. to y4m using:

	avconv -i inputfile.webm outputfile.y4m
  
 Suggestion: Don't forget to run Chromium in Incognito Mode
 
 some useful URLs:
 
 https://github.com/nplab/WebRTC-Data-Channel-Playground
 
 https://webrtchacks.com/video_replay/
 
 a sample video for testing:
 
 https://www.dropbox.com/sh/0gz5okspfma41oh/AACoKqwkon78KOoASe22c9pya?dl=0
 
 sctp dump and rtp dump:
 
 https://webrtchacks.com/facetime-finally-faces-webrtc-implementation-deep-dive/
 
 
[https://chromium.googlesource.com/chromium/src/+/refs/heads/main/docs/mac_build_instructions.md](https://chromium.googlesource.com/chromium/src/+/refs/heads/main/docs/mac_build_instructions.md)

### New version of test-app
https://github.com/tobiasfl/webrtc-test-app

  
### Deadlines
* Master thesis: 16.05.2022
