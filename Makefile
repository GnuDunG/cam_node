all: cam_node


cam_node: camera_node_raspi.c
			gcc -o cam_node camera_node_raspi.c print_header.c 

clean:
		#rm -rf *o hello