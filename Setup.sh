#This is my shell


cd /
sudo git clone https://github.com/GnuDunG/cam_node 
cd /cam_node
make -f Makefile

#sed 's/^exit0:.*$//' /etc/rc.local 
#sed '$a sudo ' Textdatei 
#cp autostart_node.sh /etc/rc3.d

echo "reboot in 10 seconds..."

#sleep(10)

