

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
 
 
### Writing/ reading tasks

 - [x] Prepare a structure of your thesis: just headings, and create table of contents.
 - [x] Subheadings for your background sections: start with what your audience should know: webrtc architecture, transport protocols, congestion controls, coupled congestion and shared bottleneck detection 
 - [x] RTP, RTCP, SCTP in detail
 - [x] congestion control mechanisms for webrtc: GCC, NADA(CISCO) 
 - [x] Come up with your research question(s)
 - [ ] Read about Scream 
 - [x] Look at papers form Julius
 - [x] Read RFC8699 (Coupled congestion control)
 - [ ] Draft Coupled Congestion control section
 - [ ] Draft WebRTC congestion controls section
 - [ ] Draft Congestion control section
 - [ ] Draft Coupled congestion algorithm design section(algorithm steps and integrating with data and video)
 - [ ] Draft about metrics to use in evaluation section (look at Safiqul's papers for which fairness index and etc. ) 
 - [ ] Describe organization of the rest of the thesis 
 - [ ] Skim paper on ensemble TCP
 - [ ] Skim paper on Congestion Manager 

### Implementation/ testing tasks
 - [ ] Video and Data flows: classes!  *NOT URGENT*
 - [ ] 1. Implement example algorithm 1 of active FSE in chromium
 - [ ] 1. Extend the webrtc app with screen sharing over media channel
 - [ ] 1. Test with video and screen sharing using only media channel 
 - [ ] 2. Implement and test FSE-NG with video and data flow
 - [ ] 3. Test algorithm 1 with video+data at the same time
 - [ ] Implement screen sharing over data channel in the webrtc app

 ### Next meeting:

Time: April 8, 2021 2:00 PM Oslo

Join Zoom Meeting
https://uio.zoom.us/j/9693289755
 
 
### Deadlines
* Essay: 01.06.2021
* Master thesis: 16.05.2022
