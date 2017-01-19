

/******************************************************************************
 *  ESW CCTV Project - Header file for message headers
 *
 *  @file:       msg_hdr.h
 *  @version:    V1.0    
 *  @date:       2017-01-17
 *  @author:     Martin Schmied
 *
 *****************************************************************************/

//Include Guard
#ifndef MSG_HDR_H
#define MSG_HDR_H

#include <stdint.h>
/**
 * Header for UDP image data beeing sent in singel picture mode.
 */
/*! @typedef struct cam_data_t
    @brief Image data message header information.
*/
typedef struct {
    uint16_t cam_id; //!<Camera ID
    uint8_t segments; //!<# of picture parts
    uint8_t segment_i; //!< i^th part
    uint32_t timestamp_low; //!<low word of timestemp in milliseconds
    uint32_t timestamp_hi; //!<hi word of timestamp in milliseconds
    uint32_t length; //!<picture length
    uint8_t *data; //!<picture
} cam_data_t;


/**
 * Listing of all defined message types in the system
 */
/*! @typedef enum control_message_T
    @brief Enumeration contains message types.
*/
typedef enum {
    AUTH_REQUEST, //!<Request authentication to access camera controls
    AUTH_SET_MASTER_FP = 10, //!<Set new master fingerprint
    AUTH_SET_PASSWORD = 20, //!<Set new password
    DISPLAY_CONFIG = 30, //!<Configure display Node
    CAM_DISP_CONFIG = 40, //!<Configure camera/display Node
    CAM_CTRL, //!<Start/stop message
    DISP_CTRL = 50, //!<Ready/sleep message
    ID = 60 //!<Request id from client (used by control node)
    } control_message_t;

/**
 * Listing of all defined response types in the system
 */
/*! @typedef enum respond_t
    @brief Enumeration contains respond types.
*/
typedef enum {
    REQ, //!<Set by sender, before op has been performed
    SUCCESS = 200, //!<Respond code if operation was succesful
    UNSET = 204, //!<Respond code if no master fingerprint is set
    UNSUPPORTED = 404, //!<If operation is not supported
    FAIL = 500, //!<Operation failed
    CAM = 600, //!<ID from Cam node
    AUT = 700, //!<ID from auth node
    DSP = 800 //!<ID from display node
    } respond_t;

/**
 * First Header for TCP Messages
 */
/*! @typedef struct msg_hdr_t
    @brief First message header structure.
*/
typedef struct {
    control_message_t mtype; //!<Kind of operation to be performed/has been performed
    respond_t resp; //!<Response on operation
} msg_hdr_t;

/**
 * Second Header for TCP Messages with display configuration data
 */
/*! @typedef struct display_config_t
    @brief Second message header for display config message
*/
typedef struct {
    uint16_t cam_id; //!<Which camera node to use
    uint16_t res_x; //!<In pixel
    uint16_t res_y; //!<In pixel
    uint16_t fps; //!<Pictures per second
} display_config_t;

/**
 * Second Header for TCP Messages with camera configuration data
 */
/*! @typedef struct cam_config_t
    @brief Second message header for camera config message
*/
typedef struct {
    display_config_t cam_cfg; //!<Cam Config eqauls Disp config
} cam_config_t;

/**
 * Listing of all defined actions for a camera
 */
/*! @typedef enum cam_action_t
    @brief Enumeration contains various actions.*/
typedef enum {
    STOP,
    START,
    READY,
    SLEEP
} cam_action_t;

/**
 * Second Header for TCP Messages with camera control data
 */
/*! @typedef struct cam_control_t
    @brief Second message header for camera control message
*/
typedef struct {
    uint16_t cam_id; //!<Which Camera node to use
    cam_action_t action; //!<Which action to perform
} cam_control_t;

#endif