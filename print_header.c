

/******************************************************************************
 *	ESW CCTV Project - Print header function	
 *
 *  @file: 		print_header.c
 *	@version:	V1.0	
 *	@date:		2017-01-17
 *	@author: 	Martin Schmied
 *
 *****************************************************************************/

#include <stdio.h>
#include "msg_hdr.h"

/******************************************************************************
 *	Print all information of a passed header to stdout
 *	@param[in] type First header struct (msg_type,resp).
 *	@param[in] control Second header struct for control.
 * 	@param[in] config Second header struct for config.
 *	@return Zero
 *****************************************************************************/
void printheader (msg_hdr_t type,cam_control_t control, cam_config_t config)
{
	printf("\n####Message Content####\n");
	printf("Message type: %d\n", type.mtype );
	printf("Respond type: %d\n\n", type.resp);
	if(type.mtype == CAM_DISP_CONFIG)
	{
		printf("Cam_config ID: %d\n", config.cam_cfg.cam_id );
		printf("Cam_config Res_x %d\n", config.cam_cfg.res_x );
		printf("Cam_config Res_y %d\n", config.cam_cfg.res_y );
		printf("Cam_config FPS: %d\n", config.cam_cfg.fps );

	}
	if(type.mtype == CAM_CTRL)
	{
		printf("Cam_control ID: %d\n", control.cam_id );
		printf("Cam_control Action: %d\n\n", control.action );
	}
	
}