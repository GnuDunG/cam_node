

/******************************************************************************
 *  ESW CCTV Project - Control-Stub for camera node
 *
 *  @file:       stub_control_node.c
 *  @version:    V1.0    
 *  @date:       2017-01-05
 *  @author:     Martin Schmied
 *
 *	@description:
 *		This program allow you to send custom control or config TCP messages 
 *		in the context of this project. The used port is predefined and may be 
 *		changed.
 *
 *****************************************************************************/

/****************************************************************** Libraries*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include "msg_hdr.h"
#include "print_header.h"


#define HOSTPORT 55600 //!<Used port for TCP communication

/******************************************************************************
 *	
 *	Main function - no arguments needed
 *	
 *****************************************************************************/
int main(int argc, char const *argv[])
{
	
	int sock_de,new_sock_de,d;
	struct sockaddr_in host_addr;
	socklen_t addrlen;
	char buffer[sizeof(msg_hdr_t)+sizeof(cam_control_t)];
	char c;
	msg_hdr_t source;
	cam_control_t cam_control;
	cam_config_t config;


	//Create Socket
	sock_de = socket(AF_INET,SOCK_STREAM,0);
	if(sock_de > 0) printf("--Socket has been created--\n");
	else printf("--ERROR: creating socket\n");

	//Customize host_addr
	host_addr.sin_family = AF_INET;
	host_addr.sin_addr.s_addr = INADDR_ANY;
	host_addr.sin_port = htons(HOSTPORT);

	//bind socket to host_addr
	bind(sock_de,(struct sockaddr*) &host_addr, sizeof(host_addr));

	//Get length
	addrlen = sizeof(struct sockaddr_in);

	printf("Listening...\n");
	//listen 
	listen(sock_de,5);

	new_sock_de = accept(sock_de,(struct sockaddr*) &host_addr, &addrlen);
	if(new_sock_de > 0) printf("--Connected!--\n");
	else printf("--Connection failed--\n");

	//Authentification Request
	printf("Sending authentication request.\n");

	source.mtype = ID;
	source.resp = REQ;

	write(new_sock_de,&source,sizeof(source));

	//Awaiting ID
	do{
		printf("Receiving ID\n");
		recv(new_sock_de,&source,sizeof(source),0);
			
	}while(source.resp != CAM);
	

	while(1)
	{	
		//User input
		printf("\n--NEW MESSAGE\n");
		printf("\nChoose Messagetype: 1 - Control  /   2 - Config\n");
		c = getchar();
		
		//Handle Messagetype
		switch(c)
		{
			case '2':

				//Prepare config msg
				source.mtype = CAM_DISP_CONFIG;
				source.resp = REQ;

				//User input
				printf("--cam_id: \t");
				scanf("%d",(int *) &config.cam_cfg.cam_id);
				printf("\n --Resulution X: \t");
				scanf("%d",(int *) &config.cam_cfg.res_x);
				printf("\n --Resulution Y: \t");
				scanf("%d",(int *) &config.cam_cfg.res_y);
				printf("\n --Frames per Second: \t");
				scanf("%d",(int *) &config.cam_cfg.fps);
				getchar();
				
				//Copy msg to buffer
				memcpy(buffer,&source,sizeof(source));
				memcpy(buffer+sizeof(source),&config,sizeof(config));

				//Send msg
				write(new_sock_de, buffer, sizeof(buffer));

				printheader(source,cam_control,config);

				printf("\n--Message sent!");
				printf("(BUFERSIZE: %d)\n", sizeof(buffer));
				break;


			case '1':

				//prepare config msg
				printf("Choose Command: 1 - START  /  2 - STOP / 3 - READY / 4 - SLEEP / 5 - NONESENSE\n");
				source.mtype = CAM_CTRL;
				source.resp = REQ;
				
				
				scanf("%d",&d);
				getchar();	//Consume new line
			
				switch(d)
				{
					case 1:
						cam_control.action = START;
						break;
					case 2:
						cam_control.action = STOP;
						break;
					case 3:
						cam_control.action = READY;
						break;
					case 4:
						cam_control.action = SLEEP;
						break;
					case 5:
						cam_control.action = 555;
						break;
				}
				
				cam_control.cam_id = 1;

				//Copy msg to buffer
				memcpy(buffer,&source,sizeof(source));
				memcpy(buffer+sizeof(source),&cam_control,sizeof(cam_control));

				//Send msg
				write(new_sock_de, buffer, sizeof(buffer));

				printheader(source,cam_control,config);

				printf("\n--Message sent!\n");
				printf("BUFERSIZE: %d\n", sizeof(buffer));
				break;
			default:
				printf("--unvalid character--\n");
				break;
		}
		
	}
	


	


	return 0;
}