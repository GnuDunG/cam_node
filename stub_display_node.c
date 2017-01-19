

/******************************************************************************
 *  ESW CCTV Project - Display-Stub for camera node
 *
 *  @file:       stub_display_node.c
 *  @version:    V1.0    
 *  @date:       2016-12-22
 *  @author:     Martin Schmied
 *
 *	@description:
 *		This programm reads image data segments from a created UDP Socket,
 *		and writes the to a jpeg file. The path to the image dump folder is
 *		predefined and may be changed.  
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
#include <poll.h>
#include "msg_hdr.h"

/******************************************************************** Defines*/
#define BUF 8192
#define SERVERPORT 55000
#define DUMP_PATH "/home/martin/Desktop/img_dump/raspi_img"


/******************************************************************************
 *	
 *	Main function - no arguments needed
 *	
 *****************************************************************************/
int main(int argc, char const *argv[])
{
	
	int sock_de,length,bytes;
	struct sockaddr_in host,client;

	FILE *fptr;

	cam_data_t cam_data;

	char img_file[100];

	char buffer[BUF];
	char buffer2[(BUF-sizeof(cam_data_t))];

	int count = 0;

	if((sock_de = socket(AF_INET, SOCK_DGRAM, 0))<0)
		{printf("--ERROR: Socket--\n");}
	else printf("--SUCCESS: Socket created--\n");

	//Customize Address Struct
	host.sin_family = AF_INET;
	host.sin_port = htons(SERVERPORT);
	host.sin_addr.s_addr = htonl(INADDR_ANY);

	//Bind socket to Address Struct
	if(bind(sock_de,(struct sockaddr *) &host,sizeof(host))<0)
		{printf("--ERROR: binding\n");}
	else printf("--SUCCESS: Binding\n");

	sprintf(img_file, "%s%d.jpeg",DUMP_PATH,count); //Create initial Image Path

	while(1)
	{
		printf("\nListening... \n");
		//open file
		fptr = fopen(img_file,"wb");
		do{
			memset(buffer,0,BUF); 	//buffer initialisieren
			length = sizeof(client);	
			bytes = recvfrom(sock_de,buffer,BUF,0,(struct sockaddr*) &client, &length);
			printf("\n\n--BYTES ARRIVED: %d\n", bytes);

			//cast header from buffer to struct
			memcpy(&cam_data,&buffer[0],sizeof(cam_data_t));
			//Display Headerinformation
			printf("#####HEADER#####\n");
			printf("CamID:\t\t %10d\n", cam_data.cam_id );
			printf("Picture Length:\t %10d\n",cam_data.length);
			printf("Segment: \t %3d of %d\n",cam_data.segment_i, cam_data.segments);
			printf("Timestamp High Word: %8d\n", cam_data.timestamp_hi );
			printf("Timestamp Low Word:  %8d\n", cam_data.timestamp_low );
			
			//Get image data out of buffer
			memcpy(&buffer2[0],&buffer[sizeof(cam_data_t)],(bytes-sizeof(cam_data_t)));
			//Write image data to file
			fwrite(buffer2,1,(bytes-sizeof(cam_data_t)),fptr); //Bytes in Fileschreiben

		}while(bytes == BUF);

		fclose(fptr);
		count++;	
		sprintf(img_file, "%s%d.jpeg",DUMP_PATH,count); //Create new Image Path
	}

	return 0;
}