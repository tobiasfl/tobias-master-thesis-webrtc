# Getting the test app:
Make sure node, git and npm is installed on the machine.
To get the repo, install npm dependencies and build the code run:

    bash setup_app.hs

The test-app repo will be placed in the $HOME directory.
To start the server run:

    bash run_app.sh

# Getting the fake video file
I have created a share link to the file on google drive. If you run the "get\_fake\_video.sh" script it will automatically be downloaded with wget and placed in $HOME. The other scripts for running chrome with fake video assumes it is placed there.

# Running Chrome with fake video, or sending a dummy file:
I assume that google chrome is already installed on the machine.
The app uses the query string in the url to know whether it should send a camera stream, a data file, or not send anything but still receive.
If the app is a video stream or data sender, it will start sending approx. 10 seconds after both peers are connected to the signalling server.
I've made three small scripts for opening chrome with the correct url, they demand that you specify the ip address of the machine hosting the app server.
The data sender will only send a bytearray of around 300mb so if the bandwidth is not limited it will finish the transfer very quickly.
The chrome log output is written to a file called log.txt.

### Run chrome as video sender example:
    bash run_chrome_video_sender.sh 10.0.0.1 

### Run chrome as data sender from same host as server example:
    bash run_chrome_video_sender.sh localhost

### Run chrome as passive receiver only:
    bash run_chrome_receiver.sh 10.0.0.2
