# Coupled congestion control for WebRTC
The ./coupling\_mechanisms directory includes the source code for the different coupling mechanisms only.

## Actually testing in chromium
All the code is dependent on the Chromium source code which can be downloaded here: `https://chromium.googlesource.com/chromium/src/+/master/docs/linux/build_instructions.md`

./usrsctp.patch and ./webrtc.patch are git patch files for the usrsctp and webrtc libraries in the chromium code.
They contain all the changes that were done to add the coupling mechanisms.

After applying the patches and compiling Chromium, you must use the --enable-features flag to run Chromium with a coupling mechanism.
The options are:
- `Fse` (Flow State Exchange)
- `FseNg` (FSE-NG)
- `FseNgV2` (Extended FSE-NG)
- `FseV2` (New coupling mechanism)

Example of running chromium with Flow State Exchange enabled:
`$/chromium/src/out/Default/chrome --enable-logging=stderr --vmodule=*/webrtc/*=1 --force-fieldtrials=WebRTC-DataChannel-Dcsctp/Disabled --enable-features=Fse`

The flag `--force-fieldtrials=WebRTC-DataChannel-Dcsctp/Disabled` disables the new dcsctp lib so that usrsctp is used instead.  
This necessary because the coupling mechanisms are only implemented and tested with usrsctp.


