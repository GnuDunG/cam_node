all: cam_node

cam_node: camera_node_raspi.o print_header.o
			gcc camera_node_raspi.o print_header.o -o cam_node

camera_node_raspi: camera_node_raspi.c
			gcc -c camera_node_raspi.c

print_header.o: print_header.c
			gcc -c print_header.c

clean:
		#rm -rf *o hello