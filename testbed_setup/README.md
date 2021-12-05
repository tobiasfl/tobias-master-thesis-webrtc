# Getting the test app:
Make sure node and npm is installed on the machine.
To get the repo, install npm dependencies and build the code run:
    bash setup_app.hs

The the test-app repo will be placed in the $HOME directory.
To start the server run:
    bash run_app.sh

# Running Chrome with fake video, or sending a dummy file:
I assume that google chrome is already installed on the machine.
The app uses the query string in the url to know whether it should send a camera stream, a data file, or not send anything but still receive.
If the app is a stream or data sender, it will start sending approx. 10 seconds after both peers are connected to the signalling server.
I've made three small scripts for opening chrome with the correct url, they demand that you specify the ip address of the machine hosting the app server.
The video sender script assumes that the fake video file is located in the current working directory.
The data sender will only send a bytearray of around 300mb so if the bandwidth is not limited it will finish the transfer very quickly.
The chrome log output is written to a file called log.txt.

### Run chrome as video sender example:
    bash run_chrome_video_sender.sh 10.0.0.1

### Run chrome as data sender from same host as server example:
    bash run_chrome_video_sender.sh localhost

### Run chrome as passive receiver only:
    bash run_chrome_receiver.sh 10.0.0.2
