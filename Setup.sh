#This is my shell

autostart_path = /etc/rc3.d

echo "Enter destination path: "
read path
mkdir $path
cd $path
git clone https://github.com/GnuDunG/cam_node 
cd cam_node
make -f Makefile
cp cam_node $autostart_path
cp autostart_node.sh $autostart_path

echo "reboot in 10 seconds..."

#sleep(10)

