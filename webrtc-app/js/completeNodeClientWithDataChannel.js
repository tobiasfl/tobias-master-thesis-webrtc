/**
 * Created by safiqul on 07.10.15.
 */


'use strict';
// Look after different browser vendors' ways of calling the getUserMedia()
// API method:
// Opera --> getUserMedia
// Chrome --> webkitGetUserMedia
// Firefox --> mozGetUserMedia
navigator.getUserMedia = navigator.getUserMedia || navigator.webkitGetUserMedia || navigator.mozGetUserMedia;

// Clean-up function:
// collect garbage before unloading browser's window
window.onbeforeunload = function(e){
    hangup();
}

console.log("RECV_DATA_STAT:timestamp,total_bytes,recv_bytes");
console.log("RECV_VIDEO_STAT:timestamp,bytesReceived");
console.log("SEND_VIDEO_STAT:timestamp,id,avail_bitrate,transmit_bitrate,actual_encoder,bytesSent,rtt,packetsSent");

// Data channel information
var sendChannel, receiveChannel;
var sendButton = document.getElementById("sendButton");
var sendTextarea = document.getElementById("dataChannelSend");
var receiveTextarea = document.getElementById("dataChannelReceive");

//Data channel file transfer info:

var liveBitrateDiv = document.querySelector('div#liveBitrate');
var bitrateDiv = document.querySelector('div#bitrate');
var fileInput = document.querySelector('input#fileInput');
var downloadAnchor = document.querySelector('a#download');
var sendProgress = document.querySelector('progress#sendProgress');
var receiveProgress = document.querySelector('progress#receiveProgress');
var statusMessage = document.querySelector('span#status');

var transferSizeInput = document.querySelector('input#transferSize')

// HTML5 <video> elements
var localVideo = document.querySelector('#localVideo');
var remoteVideo = document.querySelector('#remoteVideo');

/* File transfer */
var receivedMetadata = null; /* This is actually file metadata */
var receiveBuffer = [];
var receivedSize = 0;
var timestampStart;
sendButton.onclick = sendData;

/* Analysis of reported data */
var bytesPrev = 0;
var bytesReceivedPrev = 0;
var bytesSentPrev = 0;
var timestampPrev = 0;
var statsInterval = null;
var statsIntervalDebug = null;
var bitrateMax = 0;

/* Flags */
var requiredNumberOfHostReached = false;
var isInitiator = false;
var isStarted = false;

/* WebRTC data structures */
var pc;
var localStream;
var remoteStream;

const sleep = (milliseconds) => {
    return new Promise(resolve => setTimeout(resolve, milliseconds))
}



// PeerConnection ICE protocol configuration (either Firefox or Chrome)
var pc_config = webrtcDetectedBrowser === 'firefox' ?
    {'iceServers':[{'url':'stun:23.21.150.121'}]} : // IP address
    {'iceServers': [{'url': 'stun:10.0.3.1:3478'}]};
//{'iceServers': [{'url': 'stun:stun.l.google.com:19302'}]};
var pc_constraints = {
    'optional': [
        {'DtlsSrtpKeyAgreement': true}
    ]};


var sdpConstraints = {};

var room = 'mithun';
var socket = io.connect("http://10.0.3.1:51002");  //http://109.189.94.126:51002
var constraints = {video: true, audio: false};
socket.emit('join_room', room);

/* Signaling server interaction */
/* Three cases: created (initiator), join (new joined), joined (joined existing), full (rejected)*/

socket.on('created', function (room){
    isInitiator = true;
    navigator.getUserMedia(constraints, handleUserMedia, handleUserMediaError);
});
socket.on('join', function (room, cnt){
    if (cnt >= 2)
	requiredNumberOfHostReached = true;
});
socket.on('joined', function (room){
    requiredNumberOfHostReached = true;
    navigator.getUserMedia(constraints, handleUserMedia, handleUserMediaError);
});
socket.on('full', function (room){
    console.log("Room full, please make sure no more than two browsers are active");
    exit(1)
});
socket.on('log', function (array){
    console.log.apply(console, array);
});

function handleUserMedia(stream) {
    localStream = stream;
    attachMediaStream(localVideo, stream);
    sendMessage('got user media');
}
function handleUserMediaError(error){
    console.log('navigator.getUserMedia error: ', error);
    exit(2);
}

// Receive message from the other peer via the signaling server
socket.on('message', function (message){
    if (message === 'got user media') {
        checkAndStart();
    } else if (message.type === 'offer') {
        if (!isInitiator && !isStarted) {
            checkAndStart();
        }
        pc.setRemoteDescription(new RTCSessionDescription(message));
        doAnswer();
    } else if (message.type === 'answer' && isStarted) {
        pc.setRemoteDescription(new RTCSessionDescription(message));
    } else if (message.type === 'candidate' && isStarted) {
        var candidate = new RTCIceCandidate({sdpMLineIndex:message.label,
					     candidate:message.candidate});
        pc.addIceCandidate(candidate);
    } else if (message === 'bye' && isStarted) {
        handleRemoteHangup();
	isInitiator = true;
	requiredNumberOfHostReached = false;
    } else if (message.identifier == 'identifier') {
	console.log("handleMessage should have metadata");
        receivedMetadata = message;

	/* Reset all state related to transfer */
	receiveBuffer = [];
	receivedSize = 0;
	receiveProgress.value = 0;
	timestampStart = (new Date()).getTime();
        receiveProgress.max = receivedMetadata.size;
    }
});

function getAndSendFileInfo() {
    var file = fileInput.files[0];
    if (file == null)
	return null
    var informationFile = {
        name: file.name,
        size: file.size,
        type: file.type,
        lastModifiedDate: file.lastModifiedDate,
        identifier: "identifier"
    };
    sendMessage(informationFile);
    return file
}

function getAndSendTransferInfo() {
    var transferSize = transferSizeInput.valueAsNumber * 1000000;
    if (!isNaN(transferSize)) {
	var file_info = {
	    name: "Transfer",
	    size: transferSize,
	    type: "virtual",
	    lastModifiedDate: "never",
	    identifier: "identifier"
	};
	sendMessage(file_info);
    }
    return transferSize
}

// 2. Client-->Server
// Send message to the other peer via the signaling server
function sendMessage(message){
    socket.emit('message', message, room);
}

// Channel negotiation trigger function
function checkAndStart() {
    if (!isStarted && requiredNumberOfHostReached) {
        createPeerConnection();
        isStarted = true;
        if (isInitiator) {
            doCall();
        }
    }
}
// PeerConnection management...
function createPeerConnection() {
    try {
        pc = new RTCPeerConnection(pc_config, pc_constraints);
	//pc = new RTCPeerConnection(pc_config, null);
	if (localStream)
            pc.addStream(localStream);
        pc.onicecandidate = handleIceCandidate;
    } catch (e) {
        alert('Cannot create RTCPeerConnection object.');
        return;
    }
    pc.onaddstream = handleRemoteStreamAdded;
    pc.onremovestream = handleRemoteStreamRemoved;

    if (isInitiator) {
        try {
	    // Create a reliable data channel
            sendChannel = pc.createDataChannel("sendDataChannel",
					       {reliable: true});
	    console.log("Data channel should be created:" + sendChannel);
        } catch (e) {
            alert('Failed to create data channel. ');
	    console.log("failed to create data channel");
        }
        sendChannel.onopen = handleSendChannelStateChange;
        sendChannel.onclose = handleSendChannelStateChange;
    } else { // Joiner
        pc.ondatachannel = gotReceiveChannel;
    }

}

// Data channel management
async function sendData(size_MB=0) {

    var transferSize = getAndSendTransferInfo();

    if (size_MB != 0) {
	transferSize = size_MB * 1000000;
    }

    if (!isNaN(transferSize)) {

	var chunk_size = 32768;
	var data = new Uint8Array(chunk_size); data.fill(1);
	var total_sent = 0;

	statusMessage.textContent = '';
	downloadAnchor.textContent = '';
	sendProgress.max = transferSize;
	receiveProgress.max = transferSize;


	while (total_sent < transferSize) {

	    /* Busy wait */
	    while (sendChannel.bufferedAmount > 1000000) {
		await sleep(1)
	    }
	    var toSend = Math.min(chunk_size, transferSize - total_sent);

	    if (toSend < chunk_size)
		sendChannel.send(data.slice(0, toSend));
	    else
		sendChannel.send(data);

	    sendProgress.value = total_sent;
	    total_sent += toSend;
	}
	setTimeout(sendData, 60000, 100)
	return;
    }

    console.log("Trying to send a file");

    var file = getAndSendFileInfo();
    if (file == null) {
	console.log("You have to select a file to send!");
	return
    }
    
    if (file.size === 0) {
        bitrateDiv.innerHTML = '';
        statusMessage.textContent = 'File is empty, please select a non-empty file';
        //stop(); // Safi Changed to match stop
        return;
    }

    statusMessage.textContent = '';
    downloadAnchor.textContent = '';
    sendProgress.max = file.size;
    receiveProgress.max = file.size;
    
    var chunkSize = 163840;
    var bufferThresh = 500000;


    var fileReader = new window.FileReader();
    let offset = 0;
    fileReader.addEventListener('load', e => {

	var dataLen = e.target.result.byteLength;
	var bufferLen = sendChannel.bufferedAmount
	
	if (dataLen + bufferLen > bufferThresh) {
	    sleep(1).then(() => {
		readSlice(offset);
	    })
	} else {
	    
	    sendChannel.send(e.target.result);
	    offset += e.target.result.byteLength;
	    sendProgress.value = offset;
	    
	    
	    if (offset < file.size) {
		readSlice(offset);
	    }
	}
    });
    const readSlice = o => {
	const slice = file.slice(offset, o + chunkSize);
	fileReader.readAsArrayBuffer(slice);
    };
    readSlice(0);
}
// Handlers...
function gotReceiveChannel(event) {
    trace('Receive Channel Callback');
    receiveChannel = event.channel;  // added
    receiveChannel.binaryType = 'arraybuffer';
    receiveChannel.onmessage = handleMessage;
    receiveChannel.onopen = handleReceiveChannelStateChange;
    receiveChannel.onclose = handleReceiveChannelStateChange;

    bitrateMax = 0;
    downloadAnchor.textContent = '';
    downloadAnchor.removeAttribute('download');
    if (downloadAnchor.href) {
        URL.revokeObjectURL(downloadAnchor.href);
        downloadAnchor.removeAttribute('href');
    }
}



function handleMessage(event) {
    if (receivedMetadata == null)
	return;
    
    var file = receivedMetadata;

    /* Dont save data for virtual files */
    if (file.type != "virtual") {
	receiveBuffer.push(event.data);
    }

    receivedSize += event.data.byteLength;
    receiveProgress.value = receivedSize;

    console.log("RECV_DATA_STAT:" + Date.now() + "," + receivedSize + "," + event.data.byteLength);
    
    if (receivedSize === file.size) {
        var received = new window.Blob(receiveBuffer);

	if (file.type !== "virtual") {
            downloadAnchor.href = URL.createObjectURL(received);
            downloadAnchor.download = file.name;
            downloadAnchor.textContent =
		'Click to download \'' + file.name + '\' (' + file.size + ' bytes)';
            downloadAnchor.style.display = 'block';

	}
        var bitrate = Math.round(receivedSize * 8 /
				 ((new Date()).getTime() - timestampStart));
        bitrateDiv.innerHTML = '<strong>Download complete! Average Bitrate:</strong> ' +
            bitrate + ' kbits/sec (max: ' + bitrateMax + ' kbits/sec)';
    }
}
function handleSendChannelStateChange() {
    var readyState = sendChannel.readyState;
    trace('Send channel state is: ' + readyState);
    // If channel ready, enable user's input
    if (readyState == "open") {
        dataChannelSend.disabled = false;
        dataChannelSend.focus();
        dataChannelSend.placeholder = "";
        sendButton.disabled = false;
        fileInput.disabled = false;
	setTimeout(sendData, 100000, 100)
	statsInterval = window.setInterval(displayStats, 500);
	statsIntervalDebug = window.setInterval(getBrowserVideoStats, 50);
    } else {
        dataChannelSend.disabled = true;
        sendButton.disabled = true;
	
    }
}

function handleReceiveChannelStateChange() {
    var readyState = receiveChannel.readyState;
    trace('Receive channel state is: ' + readyState);
    if (readyState === 'open') {
        fileInput.disabled=true; // not allowing the receiver to send file
        statsInterval = window.setInterval(displayStats, 500);
	statsIntervalDebug = window.setInterval(getBrowserVideoStats, 50);
        //window.setTimeout(displayStats, 100);
        //window.setTimeout(displayStats, 300);
    }
}

// ICE candidates management
function handleIceCandidate(event) {
    console.log('handleIceCandidate event: ', event);
    if (event.candidate) {
        sendMessage({
            type: 'candidate',
            label: event.candidate.sdpMLineIndex,
            id: event.candidate.sdpMid,
            candidate: event.candidate.candidate});
    } else {
        console.log('End of candidates.');
    }
}
// Create Offer
function doCall() {
    console.log('Creating Offer...');
    pc.createOffer(setLocalAndSendMessage, onSignalingError, sdpConstraints);
}
// Signaling error handler
function onSignalingError(error) {
    console.log('Failed to create signaling message : ' + error.name);
}

// Create Answer
function doAnswer() {
    console.log('Sending answer to peer.');
    pc.createAnswer(setLocalAndSendMessage, onSignalingError, sdpConstraints);
}
// Success handler for both createOffer()
// and createAnswer()
function setLocalAndSendMessage(sessionDescription) {
    pc.setLocalDescription(sessionDescription);
    sendMessage(sessionDescription);
}
// Remote stream handlers...
function handleRemoteStreamAdded(event) {
    console.log('Remote stream added.');
    attachMediaStream(remoteVideo, event.stream);
    console.log('Remote stream attached!!.');
    remoteStream = event.stream;
}

function handleRemoteStreamRemoved(event) {
    console.log('Remote stream removed. Event: ', event);
}
// Clean-up functions...
function hangup() {
    console.log('Hanging up.');
    stop();
    sendMessage('bye');
}
function handleRemoteHangup() {
    console.log('Session terminated.');
    stop();
}
function stop() {
    isStarted = false;
    if (sendChannel) sendChannel.close();
    if (receiveChannel) receiveChannel.close();
    if (pc) pc.close();
    pc = null;
    sendButton.disabled=true;
}

// display bitrate statistics.
function displayStats() {
    var display = function(sendBitrate, receiveBitrate) {
        liveBitrateDiv.innerHTML = '<strong>Current send/receive Bitrates:</strong> ' +
            sendBitrate + ' kbps/ ' + receiveBitrate  + ' kbps   Time ' + timestampPrev ;
    };

    if (pc && (pc.iceConnectionState === 'connected' || pc.iceConnectionState === 'completed')) {
        if (webrtcDetectedBrowser === 'chrome') {
            // TODO: once https://code.google.com/p/webrtc/issues/detail?id=4321
            // lands those stats should be preferrred over the connection stats.
            pc.getStats(null, function(stats) {
                for (var key in stats) {
		    
                    var res = stats[key];
                    if (timestampPrev === res.timestamp) {
                        return;
                    }

/*		    if (res.type === 'VideoBwe') {
			console.log(res)
		    }
*/
                    if (res.type === 'googCandidatePair' &&
                        res.googActiveConnection === 'true') {
                        // calculate current bitrate

			var bytesReceived = res.bytesReceived;
			var bytesSent = res.bytesSent;

			var sendBitrate = Math.round((bytesSent - bytesSentPrev) * 8/
						     (res.timestamp - timestampPrev))
			var receiveBitrate = Math.round((bytesReceived - bytesReceivedPrev) * 8/
							(res.timestamp - timestampPrev))

			display(sendBitrate, receiveBitrate);
			
			timestampPrev = res.timestamp;
			bytesReceivedPrev = bytesReceived;
			bytesSentPrev = bytesSent;

                        if (receiveBitrate  > bitrateMax) {
                            bitrateMax = receiveBitrate ;
                        }
                    }
                }
            });
        } else {
            // Firefox currently does not have data channel stats. See
            // https://bugzilla.mozilla.org/show_bug.cgi?id=1136832
            // Instead, the bitrate is calculated based on the number of
            // bytes received.
            var bytesNow = receivedSize;
            var now = (new Date()).getTime();
            var bitrate = Math.round((bytesNow - bytesPrev) * 8 /
				     (now - timestampPrev));
            display(bitrate);
            timestampPrev = now;
            bytesPrev = bytesNow;
            if (bitrate > bitrateMax) {
                bitrateMax = bitrate;
            }
        }
    }
}

var has_printed_ssrc_information = 0;
var has_printed_local_address = 0;
function getBrowserVideoStats() {
 
    if (pc && (pc.iceConnectionState === 'connected' || pc.iceConnectionState === 'completed')) {
	//console.log(sendChannel);
	pc.getStats(null).then(stats => {
/*	    console.log("Peer connection stats\n");
            console.log(stats);
*/
	    var bweforvideo = stats["bweforvideo"];
	    var availSend = bweforvideo.googAvailableSendBandwidth;
	    var actualEnc = bweforvideo.googActualEncBitrate;
	    var targetEnc = bweforvideo.googTargetEncBitrate;
	    var transmitBitrate = bweforvideo.googTransmitBitrate;

	    var ts = Date.now();
	    for (var key in stats) {
		if (String(key).includes("Conn-video")) {
		    var obj = stats[key]
		    var id = obj["id"];
		    var bytesSent = obj["bytesSent"];
		    var bytesReceived = obj["bytesReceived"];
		    var rtt = obj["googRtt"];
		    var pktsSent = obj["packetsSent"]
		    
		    console.log("SEND_VIDEO_STAT:" + ts + "," + id
				+ "," + availSend + "," + transmitBitrate + "," +
				actualEnc + "," + bytesSent + "," + rtt + "," +
				pktsSent);

		    if (has_printed_local_address == 0) {
			console.log("LOCAL_ADDRESS:" + obj['googLocalAddress']);
			has_printed_local_address = 1;
		    }
		}
		if (String(key).includes("ssrc") && String(key).includes("recv")) {
		    var obj = stats[key]
		    var bytesReceived = obj["bytesReceived"]
		    var bytesSent = obj["bytesSent"];
		    var id = obj["transportId"]
		    console.log("RECV_VIDEO_STAT:" +  ts + "," + bytesReceived);

		    if (has_printed_ssrc_information == 0) {
			console.log("RTP_INFORMATION:" + obj['ssrc'] + "," + obj['googCodeName'] + "," + obj['transportId']);
			has_printed_ssrc_information = 1;
		    }
		}


	    }
	});
    }
}
