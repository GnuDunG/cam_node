#This is my shell

sudo mkdir /cam_node
cd /cam_node
sudo git clone https://github.com/GnuDunG/cam_node 
cd /cam_node/cam_node
make -f Makefile
sudo cp cam_node /etc
#sed 's/^exit0:.*$//' /etc/rc.local 
#sed '$a sudo ' Textdatei 
#cp autostart_node.sh /etc/rc3.d

echo "reboot in 10 seconds..."

#sleep(10)

