#This is my shell


cd /
sudo git clone https://github.com/GnuDunG/cam_node 
cd /cam_node
make -f Makefile

sed -i '1 a /cam_node/cam_node' /etc/rc.local

#sed -ie 's/exit 0//cam_node/g' /etc/rc.local 
#sed '20 a\cam_node' 
#'/\\/cam_node\\/cam_node/a \exit 0' /etc/rc.local
#cp autostart_node.sh /etc/rc3.d

echo "reboot in 10 seconds..."
sleep 10
sudo reboot


