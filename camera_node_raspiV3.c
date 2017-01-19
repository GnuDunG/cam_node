

/******************************************************************************
 *	ESW CCTV Project - Camera Node	
 *
 *  @file: 		camera_node_raspi.c
 *	@version:	V3.0	
 *	@date:		2017-01-15
 *	@author: 	Maritn Schmied
 *
 *	@description:	
 *		This function handles image and video capture as well as all the 
 *		communication from TCP conncetion establishment up to 
 *		UDP image data transfer. If no control message is received, 
 *		the system remains in default action (STOP). 
 *
 *****************************************************************************/


/****************************************************************** Libraries*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>
#include <netdb.h>
#include <poll.h>
#include <signal.h>
#include "msg_hdr.h"

/******************************************************************** Defines*/
#define DN_HOSTNAME "martin-Lenovo-IdeaPad-U510" //!< Display Node Hostname
#define CN_HOSTNAME "martin-Lenovo-IdeaPad-U510" //!< Control Node Hostname
#define CAMID 1	//<Camera ID
#define BUF 8192 //<Standard Buffersize
#define TCPPORT 55600
#define UDPPORT 55000
#define IMGBUF 2000000 //<Size for image buffer



/******************************************************************************
 *	
 *	Main function - no arguments needed
 *	
 *****************************************************************************/
int main(int argc, char const *argv[])
{
	
	int tcp_sock, udp_sock, auth = 0;
	struct sockaddr_in addr_tcp_server, addr_udp_server;
	struct hostent *cn;
	struct hostent *dn;
	char buffer[BUF];

	msg_hdr_t tcp_header;
	cam_config_t cam_config;
	cam_control_t cam_control;
	cam_data_t cam_data;
	
	//Set Default Values
	cam_config.cam_cfg.res_x = 500; 
	cam_config.cam_cfg.res_y = 500;
	cam_config.cam_cfg.fps = 0; 
	cam_control.action = STOP; 
	cam_data.cam_id = CAMID;

	struct timeval time_stamp;
	char c_resX[5], c_resY[5];

	int poll_val;
	struct pollfd poll_set[1];

	char *command;
	command = malloc(100); //Allocate memory for command line string
	sprintf(command,"raspistill -w %d -h %d -q 75 -t 100 -e jpg -o - ",cam_config.cam_cfg.res_x,cam_config.cam_cfg.res_y); //Create Default Command
	
	FILE *raspistill;
	int i;
	
	char imgbuffer[IMGBUF];
	int rd_count;
	int n_send;



	/********************************************
	 *	TCP Connection Setup 
	 *******************************************/
	
	tcp_sock = socket(AF_INET,SOCK_STREAM,0);
	addr_tcp_server.sin_family = AF_INET;
	addr_tcp_server.sin_port = htons(TCPPORT);

	inet_aton("192.168.0.12",&addr_tcp_server.sin_addr);
	
	/*cn = gethostbyname(CN_HOSTNAME);
	if (cn == NULL)
	{
		printf("--No Host found (TCP)--\n");
		return -1;
	}	
	memcpy(&addr_tcp_server.sin_addr, cn->h_addr_list[0], cn->h_length);
	*/

	if(connect(tcp_sock,(struct sockaddr *) &addr_tcp_server,sizeof(addr_tcp_server)) == -1)
	{
		perror(" 'CONNECT' ");
	}
	printf("-- TCP Connected --\n");

	poll_set[0].fd = tcp_sock;
	poll_set[0].events = POLLIN;

	

		/****************************************
		 *	Identify at Control Node
		 ***************************************/

		printf("Waiting for identification request...\n");	
		do{

			recv(tcp_sock,&buffer, 8, 0);
			memcpy(&tcp_header,&buffer[0],8);
			if (tcp_header.mtype == ID)
			{
				tcp_header.resp = CAM;
				printf("...replying with ID (CAM%d)\n", CAMID );
				send(tcp_sock,&tcp_header,sizeof(tcp_header),0);
				auth = 1;
			}
			else 
			{
				printf("Messagetype not valid.\n");
				printf("Waiting for identification request...\n");
			}

		}while(auth == 0);


    
	/********************************************
	 *	UDP Connection Setup 
	 *******************************************/

	udp_sock = socket(AF_INET,SOCK_DGRAM,0);
	addr_udp_server.sin_family = AF_INET;
	addr_udp_server.sin_port = htons(UDPPORT);
/*
	dn = gethostbyname(DN_HOSTNAME);
	if (dn == NULL)
	{
		printf("--No Host found (UDP)--\n");
		return -1;
	}	
	memcpy(&addr_udp_server.sin_addr, dn->h_addr_list[0], dn->h_length);
	*/

	inet_aton("192.168.0.12",&addr_udp_server.sin_addr);



	
	/********************************************
	 *	Endless "working" loop
	 *******************************************/

	while(1){
	
		/****************************************
		 *	Control Camera
		 ***************************************/
		if(cam_control.action == START)
		{
			if(cam_config.cam_cfg.fps > 0)
			{
				/********************************
		 		*	Start Video
		 		********************************/
				pid_t pid = fork(); //Create new process for video capture
				if (pid < 0)
				{
					perror(" 'Fork' ");
					return -1;
				}
				else if (pid == 0)
			    {
			    	//Child process

			    	setsid(); //Create new process group to get group ID
					system(command); //Execute command in new shell

					return 0;
			    }

				while((poll_val = poll(poll_set,1,1))==0) 
				{
					//Wait for new TCP Message
				}

				killpg(pid, SIGKILL); //Kill child process group

			}
			else
			{						
				/********************************
		 		*	Start Picture Loop
		 		********************************/
				while((poll_val = poll(poll_set,1,1))==0)
				{				
					raspistill = popen(command,"r"); //Execute command and pipe output
					if (raspistill == NULL)
					{
						printf("--ERROR popen: %d\n",raspistill);
					}

					memset(imgbuffer,0,IMGBUF); //Initialize image buffer
					rd_count = fread(imgbuffer,1,IMGBUF,raspistill);

					cam_data.length = rd_count; //Get picture size
					cam_data.segments = cam_data.length/(BUF-sizeof(cam_data)) + 1; //Calculate Segement amount				

					//Send Segments to Display
					for(i = 1; i <= cam_data.segments; i++)
					{
						cam_data.segment_i = i;
						gettimeofday(&time_stamp,NULL);
						cam_data.timestamp_hi = time_stamp.tv_sec;
						cam_data.timestamp_low = time_stamp.tv_usec;

						memset(buffer,0,BUF); //Initialize send-buffer
						memcpy(&buffer[0],&cam_data,sizeof(cam_data)); //Copy header to send-buffer
						memcpy(&buffer[sizeof(cam_data)],&imgbuffer[((i-1)*(BUF-sizeof(cam_data_t)))],(BUF-sizeof(cam_data))); //Append image data segment to send-buffer

						//Set number of available bytes in case of EOF
						rd_count = rd_count - (BUF-sizeof(cam_data));
						if(rd_count <= 0)
						{
							n_send = rd_count + BUF;
						}
						else
						{
							n_send = BUF;
						}

						//Send specified amount of bytes to display
						if(sendto(udp_sock,buffer,n_send,0,(struct sockaddr *) &addr_udp_server, sizeof(addr_udp_server)) <0 )
						{
							perror(" 'sendto': ");
						}	
					 							
						printf("Segment #%d of %d is sent.\n", cam_data.segment_i, cam_data.segments);
						printf("Packetsize: %d Bytes\n",n_send );
					} 
					
				}
			}
			
		}
		else if(cam_control.action == STOP)
		{
			/************************************
		 	*	STOP MODE
		 	************************************/
			while((poll_val = poll(poll_set,1,1))==0)
			{
				//Wait for new TCP Message
			}			
		}
		else
		{	
			printf("--Unknown State - Exiting--\n");
			return -1;
		}


		



		/****************************************
		 *	New TCP Message Available
		 ***************************************/
		
		if(poll_val < 0)
		{
			perror(" 'polling' ");
		}
		else if(poll_val > 0)
		{
			printf("\n--New TCP Message Received...\n");
			
			memset(buffer,0,BUF); //Initialize buffer
			rd_count = read(tcp_sock,&buffer,16);

			memcpy(&tcp_header,&buffer[0],8); //Get first header struct

			if(tcp_header.mtype == CAM_DISP_CONFIG)
			{
				printf("  Message Type: Config\n");
				memcpy(&cam_config,&buffer[8],8); //Get second header struct -> config

				//Check for unvalid values
				if(cam_config.cam_cfg.res_x < 0 || cam_config.cam_cfg.res_x > 2592 || cam_config.cam_cfg.res_y < 0 || cam_config.cam_cfg.res_y > 1944)
				{
					printf("Resolution out of bounderies!\n");
					tcp_header.resp = FAIL;
					if(send(tcp_sock,&tcp_header,sizeof(tcp_header),0) < 0)
					{
						perror(" 'send': ");
					}
				}
				else if(cam_config.cam_cfg.fps < 0 || cam_config.cam_cfg.fps > 30)
				{
					printf("FPS out of bounderies!\n");
					tcp_header.resp = FAIL;
					if(send(tcp_sock,&tcp_header,sizeof(tcp_header),0) < 0)
					{
						perror(" 'send': ");
					}					
				}
				else
				{					
					tcp_header.resp = SUCCESS;
					send(tcp_sock,&tcp_header,sizeof(tcp_header),0);
				}
				
				//Create new Command String
				if(cam_config.cam_cfg.fps == 0)
				{
					sprintf(command,"raspistill -w %d -h %d -q 75 -t 100 -e jpg -o - ",cam_config.cam_cfg.res_x,cam_config.cam_cfg.res_y);
				}
				else
				{	
					sprintf(command,"raspivid -o - -t 0 -n -w %d -h %d -fps %d | cvlc -vvv stream:///dev/stdin --sout '#rtp{sdp=rtsp://:8554/}' :demux=h264",cam_config.cam_cfg.res_x,cam_config.cam_cfg.res_y,cam_config.cam_cfg.fps);
				}
				
			}		
			else if(tcp_header.mtype == CAM_CTRL)
			{
				printf("  Message Type: Control\n");
				memcpy(&cam_control,&buffer[8],sizeof(cam_control_t)); //Get second header struct -> control
				
				//Handel actions
				if(cam_control.action == READY || cam_control.action == SLEEP)
				{
					printf("  Camera Action Ready/Sleep -> set to Stop\n");
					cam_control.action = STOP;
					tcp_header.resp = SUCCESS;
					send(tcp_sock,&tcp_header,sizeof(tcp_header),0);
				}
				else if(cam_control.action == START || cam_control.action == STOP)
				{
					tcp_header.resp = SUCCESS;
					send(tcp_sock,&tcp_header,sizeof(tcp_header),0);
				}
				else
				{
					printf("  Unsupported Camera Action!\n");
					tcp_header.resp = UNSUPPORTED;
					send(tcp_sock,&tcp_header,sizeof(tcp_header),0);
					cam_control.action = STOP;
				}
			}
			else
			{
				printf("--Unknown Messagetype--\n");
				tcp_header.resp = UNSUPPORTED;
				if(send(tcp_sock,&tcp_header,sizeof(tcp_header),0) < 0)
				{
						perror(" 'send': ");
				}
			}

			printheader(tcp_header,cam_control,cam_config); //Print message header to console
			
		}
	}

	return 0;
}