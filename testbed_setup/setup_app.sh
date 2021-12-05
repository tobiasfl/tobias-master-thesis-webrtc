#Assumes Node is installed
APP_LOC=$HOME

cd $APP_LOC
git clone https://github.com/tobiasfl/webrtc-test-app.git
cd $APP_LOC/webrtc-test-app
npm install
npm run build
