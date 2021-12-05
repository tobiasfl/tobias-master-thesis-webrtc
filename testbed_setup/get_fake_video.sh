
echo "downloading fake video file"

wget --load-cookies /tmp/cookies.txt "https://docs.google.com/uc?export=download&confirm=$(wget --quiet --save-cookies /tmp/cookies.txt --keep-session-cookies --no-check-certificate 'https://docs.google.com/uc?export=download&id=19cGMea7m8MPC3NaekR4bSxeWU14jzGM2' -O- | sed -rn 's/.*confirm=([0-9A-Za-z_]+).*/\1\n/p')&id=19cGMea7m8MPC3NaekR4bSxeWU14jzGM2" -O $HOME/KristenAndSara_1280x720_60.y4m && rm -rf /tmp/cookies.txt

