

/******************************************************************************
 *	ESW CCTV Project - Header for Print header function	
 *
 *  @file: 		print_header.h
 *	@version:	V1.0	
 *	@date:		2017-01-17
 *	@author: 	Martin Schmied
 *
 *****************************************************************************/

//Include Guard
#ifndef PRINT_HEADER_H
#define PRINT_HEADER_H

/******************************************************************************
 *	Print all information of a passed header to stdout
 *	@param[in] type First header struct (msg_type,resp).
 *	@param[in] control Second header struct for control.
 * 	@param[in] config Second header struct for config.
 *****************************************************************************/

void printheader (msg_hdr_t type,cam_control_t control, cam_config_t config);

#endif
