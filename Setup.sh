#This is my shell


echo "Enter destination path: "
read path
mkdir $path
cd $path
git clone https://github.com/GnuDunG/cam_node 
cd cam_node
make -f Makefile
cp cam_node /etc/rc3.d
cp autostart_node.sh /etc/rc3.d

echo "reboot in 10 seconds..."

#sleep(10)

