#!/bin/bash

cd /
#clone repository
sudo git clone https://github.com/GnuDunG/cam_node 
cd /cam_node
#compile
make -f Makefile

#get vlc to run as root
sed -i 's/geteuid/getppid/' /usr/bin/vlc
#autostart entry in rc.local shell
sed -i '1 a /cam_node/cam_node' /etc/rc.local

echo "reboot in 10 seconds..."
sleep 10
sudo reboot


