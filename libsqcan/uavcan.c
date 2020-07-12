/*
 * Copyright (C) 2017 Konstantin Burdinov <sainquake@gmail.com>
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <stdio.h>
#include <stdlib.h>

#include "canard.h"
#include "canard_stm32.h"
#include "uavcan.h"
#include "stm32f1xx_hal.h"
#include "main.h"
#include "sq_nmea.h"
#include "sq_HMC5883L.h"
#include "sq_MS4525DO.h"

#define CANARD_SPIN_PERIOD   500
#define PUBLISHER_PERIOD_mS     250
            
static CanardInstance g_canard;                //The library instance
static uint8_t g_canard_memory_pool[1024];     //Arena for memory allocation, used by the library
int16_t rc_pwm[10] = {0,0,0,0,0,0,0,0,0,0};

const volatile int32_t *fl=(const volatile int32_t *)STORAGE_PAGE;

///					A1 A2 B1 B2
uint32_t test[4] = {0, 0, 0, 0};

char* name;

param_t parameters[] =
{
    {(uint8_t*)"ID", 			50, 0,	100, 	50},
	//
	{(uint8_t*)"A1 ch/mode", 	0, 	-2,	10, 	-1},
	{(uint8_t*)"A2 ch/mode", 	0, 	-2,	10, 	-1},
	{(uint8_t*)"B1 ch/mode", 	0, 	-2,	10, 	-1},
	{(uint8_t*)"B2 ch/mode", 	0, 	-2,	10, 	-1},
	//
    {(uint8_t*)"A1 min", 	0, 	900,	2000,	 1000},
    {(uint8_t*)"A2 min", 	0, 	900,	2000,	 1000},
	{(uint8_t*)"B1 min", 	0, 	900,	2000,	 1000},
	{(uint8_t*)"B2 min", 	0, 	900,	2000,	 1000},
	//
    {(uint8_t*)"A1 max", 	0, 	900,	2000,	 2000},
    {(uint8_t*)"A2 max", 	0, 	900,	2000,	 2000},
	{(uint8_t*)"B1 max", 	0, 	900,	2000,	 2000},
	{(uint8_t*)"B2 max", 	0, 	900,	2000,	 2000},
	//
    {(uint8_t*)"A1 default",0, 	900,	2000,	 1000},
    {(uint8_t*)"A2 default",0, 	900,	2000,	 1000},
	{(uint8_t*)"B1 default",0, 	900,	2000,	 1000},
	{(uint8_t*)"B2 default",0, 	900,	2000,	 1000},
	{(uint8_t*)"name",		0, 	0,		100,	 0}		//17
};
uint8_t param_num = 1+4+4+4+4+1;

void makeNodeStatusMessage(uint8_t buffer[UAVCAN_NODE_STATUS_MESSAGE_SIZE]);
void readUniqueID(uint8_t* out_uid);
 

//////////////////////////////////////////////////////////////////////////////////////

bool shouldAcceptTransfer(const CanardInstance* ins,
                          uint64_t* out_data_type_signature,
                          uint16_t data_type_id,
                          CanardTransferType transfer_type,
                          uint8_t source_node_id)
{
    if ((transfer_type == CanardTransferTypeRequest) &&(data_type_id == UAVCAN_GET_NODE_INFO_DATA_TYPE_ID))
    {
        *out_data_type_signature = UAVCAN_GET_NODE_INFO_DATA_TYPE_SIGNATURE;
        return true;
    }
    if (data_type_id == UAVCAN_EQUIPMENT_ESC_RAWCOMMAND_ID)
    {
        *out_data_type_signature = UAVCAN_EQUIPMENT_ESC_RAWCOMMAND_SIGNATURE;
        return true;
    }
    if (data_type_id == UAVCAN_PROTOCOL_PARAM_GETSET_ID)
    {
        *out_data_type_signature = UAVCAN_PROTOCOL_PARAM_GETSET_SIGNATURE;
        return true;
    }
    if (data_type_id == UAVCAN_PROTOCOL_PARAM_EXECUTEOPCODE_ID)
    {
            *out_data_type_signature = UAVCAN_PROTOCOL_PARAM_EXECUTEOPCODE_SIGNATURE;
            return true;
    }
    return false;
}

//////////////////////////////////////////////////////////////////////////////////////

void onTransferReceived(CanardInstance* ins, CanardRxTransfer* transfer)
{
    if ((transfer->transfer_type == CanardTransferTypeRequest) && (transfer->data_type_id == UAVCAN_GET_NODE_INFO_DATA_TYPE_ID))
    {
        getNodeInfoHandleCanard(transfer);
    } 

    if (transfer->data_type_id == UAVCAN_EQUIPMENT_ESC_RAWCOMMAND_ID)
    {
    	HAL_GPIO_TogglePin(LED_GPIO_Port,LED_Pin);
        rawcmdHandleCanard(transfer);
    }

    if (transfer->data_type_id == UAVCAN_PROTOCOL_PARAM_GETSET_ID)
    {
        getsetHandleCanard(transfer);
    }
    
    // SAVE TO FLASH COMMAND (0)
    if (transfer->data_type_id == UAVCAN_PROTOCOL_PARAM_EXECUTEOPCODE_ID)
    {

    	saveHandle(transfer);
    	//updateStorage(transfer);
    }

}

void getNodeInfoHandleCanard(CanardRxTransfer* transfer)
{
        uint8_t buffer[UAVCAN_GET_NODE_INFO_RESPONSE_MAX_SIZE];
        memset(buffer,0,UAVCAN_GET_NODE_INFO_RESPONSE_MAX_SIZE);
        uint16_t len = makeNodeInfoMessage(buffer);
        canardRequestOrRespond(&g_canard,
                               transfer->source_node_id,
                               UAVCAN_GET_NODE_INFO_DATA_TYPE_SIGNATURE,
                               UAVCAN_GET_NODE_INFO_DATA_TYPE_ID,
                               &transfer->transfer_id,
                               transfer->priority,
                               CanardResponse,
                               &buffer[0],
                               (uint16_t)len);
}





void uavcanInit(void)
{
    CanardSTM32CANTimings timings;
    int result = canardSTM32ComputeCANTimings(HAL_RCC_GetPCLK1Freq(), CAN_SPEED, &timings);
    if (result)
    {
        __ASM volatile("BKPT #01");
    }
    result = canardSTM32Init(&timings, CanardSTM32IfaceModeNormal);
    if (result)
    {
        __ASM volatile("BKPT #01");
    }
 
    canardInit(&g_canard,                         // Uninitialized library instance
               g_canard_memory_pool,              // Raw memory chunk used for dynamic allocation
               sizeof(g_canard_memory_pool),      // Size of the above, in bytes
               onTransferReceived,                // Callback, see CanardOnTransferReception
               shouldAcceptTransfer,              // Callback, see CanardShouldAcceptTransfer
               NULL);
 
    for(int i = 0; i < param_num; i++){
        	if(fl[i] == 0xffffffff)
        		parameters[i].val = parameters[i].defval;
        	else
        		parameters[i].val = fl[i];
    }
    /*
    if(((uint32_t)fl[0])>150)
    	canardSetLocalNodeID(&g_canard, NODE_ID);
    else
    	canardSetLocalNodeID(&g_canard, fl[0]);//NODE_ID);

	*/
    canardSetLocalNodeID(&g_canard, parameters[0].val);


    //parameters[1].val = fl[1];
    //parameters[2].val = fl[2];
    setPWM(A1, 0);
    setPWM(A2, 0);
    setPWM(B1, 0);
    setPWM(B2, 0);


}

void sendCanard(void)
{
  const CanardCANFrame* txf = canardPeekTxQueue(&g_canard); 
  while(txf)
    {
        const int tx_res = canardSTM32Transmit(txf);
        if (tx_res < 0)                  // Failure - drop the frame and report
        {
            __ASM volatile("BKPT #01");  // TODO: handle the error properly
        }
        if(tx_res > 0)
        {
            canardPopTxQueue(&g_canard);
        }
        txf = canardPeekTxQueue(&g_canard); 
    }
}

void receiveCanard(void)
{
    CanardCANFrame rx_frame;
    int res = canardSTM32Receive(&rx_frame);
    if(res)
    {
        canardHandleRxFrame(&g_canard, &rx_frame, HAL_GetTick() * 1000);
    }    
}

void spinCanard(void)
{  
    static uint32_t spin_time = 0;
    if(HAL_GetTick() < spin_time + CANARD_SPIN_PERIOD) return;  // rate limiting
    spin_time = HAL_GetTick();
    HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_12);   
    
    uint8_t buffer[UAVCAN_NODE_STATUS_MESSAGE_SIZE];    
    static uint8_t transfer_id = 0;                           // This variable MUST BE STATIC; refer to the libcanard documentation for the background
    makeNodeStatusMessage(buffer);  
    canardBroadcast(&g_canard, 
                    UAVCAN_NODE_STATUS_DATA_TYPE_SIGNATURE,
                    UAVCAN_NODE_STATUS_DATA_TYPE_ID,
                    &transfer_id,
                    CANARD_TRANSFER_PRIORITY_LOW,
                    buffer, 
                    UAVCAN_NODE_STATUS_MESSAGE_SIZE);                         //some indication
    
}

void publishCanard(void)
{  
    static uint32_t publish_time = 0;
    static int step = 0;
    if(HAL_GetTick() < publish_time + PUBLISHER_PERIOD_mS) {return;} // rate limiting
    publish_time = HAL_GetTick();
  
    uint8_t buffer[UAVCAN_PROTOCOL_DEBUG_KEYVALUE_MESSAGE_SIZE];
    memset(buffer,0x00,UAVCAN_PROTOCOL_DEBUG_KEYVALUE_MESSAGE_SIZE);
    step++;
    if(step == 256) 
    {
        step = 0;
    }
  /*
    float val = sine_wave[step];
    static uint8_t transfer_id = 0;
    canardEncodeScalar(buffer, 0, 32, &val);
    memcpy(&buffer[4], "sin", 3);    
    canardBroadcast(&g_canard, 
                    UAVCAN_PROTOCOL_DEBUG_KEYVALUE_SIGNATURE,
                    UAVCAN_PROTOCOL_DEBUG_KEYVALUE_ID,
                    &transfer_id,
                    CANARD_TRANSFER_PRIORITY_LOW,
                    &buffer[0], 
                    7);
    memset(buffer,0x00,UAVCAN_PROTOCOL_DEBUG_KEYVALUE_MESSAGE_SIZE);
  
    val = step;
    canardEncodeScalar(buffer, 0, 32, &val);
    memcpy(&buffer[4], "stp", 3);  
    canardBroadcast(&g_canard, 
                    UAVCAN_PROTOCOL_DEBUG_KEYVALUE_SIGNATURE,
                    UAVCAN_PROTOCOL_DEBUG_KEYVALUE_ID,
                    &transfer_id,
                    CANARD_TRANSFER_PRIORITY_LOW,
                    &buffer[0], 
                    7);
                    */
    	float val = sine_wave[step];
        static uint8_t transfer_id = 0;
        canardEncodeScalar(buffer, 0, 32, &T);
        memcpy(&buffer[4], "tmp", 3);
        canardBroadcast(&g_canard,
                        UAVCAN_PROTOCOL_DEBUG_KEYVALUE_SIGNATURE,
                        UAVCAN_PROTOCOL_DEBUG_KEYVALUE_ID,
                        &transfer_id,
                        CANARD_TRANSFER_PRIORITY_LOW,
                        &buffer[0],
                        7);
        memset(buffer,0x00,UAVCAN_PROTOCOL_DEBUG_KEYVALUE_MESSAGE_SIZE);

        val = step;
        canardEncodeScalar(buffer, 0, 32, &D);
        memcpy(&buffer[4], "pre", 3);
        canardBroadcast(&g_canard,
                        UAVCAN_PROTOCOL_DEBUG_KEYVALUE_SIGNATURE,
                        UAVCAN_PROTOCOL_DEBUG_KEYVALUE_ID,
                        &transfer_id,
                        CANARD_TRANSFER_PRIORITY_LOW,
                        &buffer[0],
                        7);
}

void publishFix(void){
    static uint32_t publish_time = 0;
    //static int step = 0;
    //if(HAL_GetTick() < publish_time + PUBLISHER_PERIOD_mS) {return;} // rate limiting
    publish_time = HAL_GetTick();

    uint8_t buffer[UAVCAN_EQUIPMENT_GNSS_FIX_MESSAGE_SIZE];
    memset(buffer,0x00,UAVCAN_EQUIPMENT_GNSS_FIX_MESSAGE_SIZE);


    	//float val = sine_wave[step];
        static uint8_t transfer_id = 0;

        uint32_t offset=0;

        uint64_t time = publish_time*1000;
        //offset+=56;// uint56 UNKNOWN = 0
        canardEncodeScalar(buffer, offset, 56, &time); // truncated uint56 usec     # Microsecond
        offset+=56;

        uint64_t time2 = publish_time*1000;
        //offset+=56;// uint56 UNKNOWN = 0
        canardEncodeScalar(buffer, offset, 56, &time2); // truncated uint56 usec     # Microsecond
        offset+=56;

        uint8_t gnss_time_standard = 3;
        canardEncodeScalar(buffer, offset, 3, &gnss_time_standard);//uint3 gnss_time_standard
        offset+=3;

        offset+=5;//void13   # Reserved space

        //uint8 NUM_LEAP_SECONDS_UNKNOWN = 0
        //uint8 num_leap_seconds
        uint8_t num_leap_seconds = 0;
        canardEncodeScalar(buffer, offset, 8, &num_leap_seconds);//uint3 gnss_time_standard
        offset+=8;

		/*#
		# Position and velocity solution
		#*/

        //int37 longitude_deg_1e8            # Longitude degrees multiplied by 1e8 (approx. 1 mm per LSB) 5575442000;///;
        int64_t longitude_deg_1e8 = Fix2.longitude_deg_1e8;
        canardEncodeScalar(buffer, offset, 37, &longitude_deg_1e8);
        offset+=37;

        //int37 latitude_deg_1e8             # Latitude degrees multiplied by 1e8 (approx. 1 mm per LSB on equator)  4874332900;//
        int64_t latitude_deg_1e8 = Fix2.latitude_deg_1e8;
        canardEncodeScalar(buffer, offset, 37, &latitude_deg_1e8);
        offset+=37;

        //int27 height_ellipsoid_mm          # Height above ellipsoid in millimeters
        int32_t height_ellipsoid_mm = Fix2.height_ellipsoid_mm;
        canardEncodeScalar(buffer, offset, 27, &height_ellipsoid_mm);
        offset+=27;

        //int27 height_msl_mm                # Height above mean sea level in millimeters
        int32_t height_msl_mm = Fix2.height_msl_mm;
        canardEncodeScalar(buffer, offset, 27, &height_msl_mm);
        offset+=27;

        //float32[3+] ned_velocity            # NED frame (north-east-down) in meters per second
        float ned_velocity[3] = {3,4,5};
        uint16_t ne_vel[3];
        ne_vel[0] = canardConvertNativeFloatToFloat16(Fix2.ned_velocity[0]);
        ne_vel[1] = canardConvertNativeFloatToFloat16(Fix2.ned_velocity[1]);
        ne_vel[2] = canardConvertNativeFloatToFloat16(Fix2.ned_velocity[2]);

        canardEncodeScalar(buffer, offset, 16, &ne_vel[0]);
        offset+=16;
        canardEncodeScalar(buffer, offset, 16, &ne_vel[1]);
        offset+=16;
        canardEncodeScalar(buffer, offset, 16, &ne_vel[2]);
        offset+=16;

        uint8_t sats_used = Fix2.sats_used;
        canardEncodeScalar(buffer, offset, 6, &sats_used);
        offset+=6;//uint6 sats_used
		//uint2 status
        uint8_t status=Fix2.status;
        canardEncodeScalar(buffer, offset, 2, &status);
        offset+=2;

//float16 pdop
        //float pdopf = 10;
        uint16_t pdop = canardConvertNativeFloatToFloat16(Fix2.pdop);
        canardEncodeScalar(buffer, offset, 16, &pdop);
        offset+=16;
//ECEFPositionVelocity[<=1] ecef_position_velocity

        //376
       /* //float32[3] velocity_xyz            # XYZ velocity in m/s
        float velocity_xyz[3] = {5,6,7};
        canardEncodeScalar(buffer, offset, 32, &velocity_xyz[0]);
        offset+=32;
        canardEncodeScalar(buffer, offset, 32, &velocity_xyz[1]);
        offset+=32;
        canardEncodeScalar(buffer, offset, 32, &velocity_xyz[2]);
        offset+=32;

        //int36[3] position_xyz_mm           # XYZ-axis coordinates in mm
        long long position_xyz_mm[3] = {5,6,7};
        canardEncodeScalar(buffer, offset, 36, &position_xyz_mm[0]);
        offset+=36;
        canardEncodeScalar(buffer, offset, 36, &position_xyz_mm[1]);
        offset+=36;
        canardEncodeScalar(buffer, offset, 36, &position_xyz_mm[2]);
        offset+=36;

        offset+=6;//void6                              # Aligns the following array at byte boundary
*/
        //float16[<=36] covariance           # Position and velocity covariance in the ECEF frame. Units are m^2 for position,
        //                                   # (m/s)^2 for velocity, and m^2/s for position/velocity.
        //uint16_t covariance[36];
        /*for(int i=0;i<36;i++){
        	covariance[i] = canardConvertNativeFloatToFloat16( (float)i );
			canardEncodeScalar(buffer, offset, 16, &covariance[i]);
			offset+=16;
        }*/

        canardBroadcast(&g_canard,
        				UAVCAN_EQUIPMENT_GNSS_FIX_SIGNATURE,
						UAVCAN_EQUIPMENT_GNSS_FIX_ID,
                        &transfer_id,
						CANARD_TRANSFER_PRIORITY_MEDIUM,
                        &buffer[0],
						UAVCAN_EQUIPMENT_GNSS_FIX_MESSAGE_SIZE);
}


void publishFix2(void){
    static uint32_t publish_time = 0;
    //static int step = 0;
    if(HAL_GetTick() < publish_time + PUBLISHER_PERIOD_mS) {return;} // rate limiting
    publish_time = HAL_GetTick();

    uint8_t buffer[UAVCAN_EQUIPMENT_GNSS_FIX2_MESSAGE_SIZE];
    memset(buffer,0x00,UAVCAN_EQUIPMENT_GNSS_FIX2_MESSAGE_SIZE);


    	//float val = sine_wave[step];
        static uint8_t transfer_id = 0;

        uint32_t offset=0;

        uint64_t time = publish_time*1000;
        //offset+=56;// uint56 UNKNOWN = 0
        canardEncodeScalar(buffer, offset, 56, &time); // truncated uint56 usec     # Microsecond
        offset+=56;

        uint64_t time2 = publish_time*1000;
        //offset+=56;// uint56 UNKNOWN = 0
        canardEncodeScalar(buffer, offset, 56, &time2); // truncated uint56 usec     # Microsecond
        offset+=56;

        uint8_t gnss_time_standard = 3;
        canardEncodeScalar(buffer, offset, 3, &gnss_time_standard);//uint3 gnss_time_standard
        offset+=3;

        offset+=13;//void13   # Reserved space

        //uint8 NUM_LEAP_SECONDS_UNKNOWN = 0
        //uint8 num_leap_seconds
        uint8_t num_leap_seconds = 0;
        canardEncodeScalar(buffer, offset, 8, &num_leap_seconds);//uint3 gnss_time_standard
        offset+=8;

		/*#
		# Position and velocity solution
		#*/

        //int37 longitude_deg_1e8            # Longitude degrees multiplied by 1e8 (approx. 1 mm per LSB) 5575442000;///;
        int64_t longitude_deg_1e8 = Fix2.longitude_deg_1e8;
        canardEncodeScalar(buffer, offset, 37, &longitude_deg_1e8);
        offset+=37;

        //int37 latitude_deg_1e8             # Latitude degrees multiplied by 1e8 (approx. 1 mm per LSB on equator)  4874332900;//
        int64_t latitude_deg_1e8 = Fix2.latitude_deg_1e8;
        canardEncodeScalar(buffer, offset, 37, &latitude_deg_1e8);
        offset+=37;

        //int27 height_ellipsoid_mm          # Height above ellipsoid in millimeters
        int32_t height_ellipsoid_mm = Fix2.height_ellipsoid_mm;
        canardEncodeScalar(buffer, offset, 27, &height_ellipsoid_mm);
        offset+=27;

        //int27 height_msl_mm                # Height above mean sea level in millimeters
        int32_t height_msl_mm = Fix2.height_msl_mm;
        canardEncodeScalar(buffer, offset, 27, &height_msl_mm);
        offset+=27;

        //float32[3] ned_velocity            # NED frame (north-east-down) in meters per second
        float ned_velocity[3] = {0,0,0};
        canardEncodeScalar(buffer, offset, 32, &ned_velocity[0]);
        offset+=32;
        canardEncodeScalar(buffer, offset, 32, &ned_velocity[1]);
        offset+=32;
        canardEncodeScalar(buffer, offset, 32, &ned_velocity[2]);
        offset+=32;

        uint8_t sats_used=Fix2.sats_used;
        canardEncodeScalar(buffer, offset, 6, &sats_used);
        offset+=6;//uint6 sats_used
		//uint2 status
        uint8_t status=Fix2.status;
        canardEncodeScalar(buffer, offset, 2, &status);
        offset+=2;

//368
//uint4 mode
        uint8_t mode=0;
        canardEncodeScalar(buffer, offset, 4, &mode);
        offset+=4;

//uint6 sub_mode
        uint8_t sub_mode=1;
        canardEncodeScalar(buffer, offset, 6, &sub_mode);
        offset+=6;

        //offset+=16;
//float16[<=36] covariance    # Position and velocity covariance. Units are
        uint16_t covariance[36];
        for(int i=0;i<36;i++){
        	covariance[i] = i+10;//canardConvertNativeFloatToFloat16( (float)(i+10) );
			canardEncodeScalar(buffer, offset, 16, &covariance[i]);
			offset+=16;
        }
//float16 pdop
        /*float pdopf = 10;
        uint16_t pdop = 1;//canardConvertNativeFloatToFloat16(pdopf);
        canardEncodeScalar(buffer, offset, 16, &pdop);
        offset+=16;*/
//ECEFPositionVelocity[<=1] ecef_position_velocity

       /* //float32[3] velocity_xyz            # XYZ velocity in m/s
        float velocity_xyz[3] = {5,6,7};
        canardEncodeScalar(buffer, offset, 32, &velocity_xyz[0]);
        offset+=32;
        canardEncodeScalar(buffer, offset, 32, &velocity_xyz[1]);
        offset+=32;
        canardEncodeScalar(buffer, offset, 32, &velocity_xyz[2]);
        offset+=32;

        //int36[3] position_xyz_mm           # XYZ-axis coordinates in mm
        long long position_xyz_mm[3] = {5,6,7};
        canardEncodeScalar(buffer, offset, 36, &position_xyz_mm[0]);
        offset+=36;
        canardEncodeScalar(buffer, offset, 36, &position_xyz_mm[1]);
        offset+=36;
        canardEncodeScalar(buffer, offset, 36, &position_xyz_mm[2]);
        offset+=36;

        offset+=6;//void6                              # Aligns the following array at byte boundary
*/
        //float16[<=36] covariance           # Position and velocity covariance in the ECEF frame. Units are m^2 for position,
        //                                   # (m/s)^2 for velocity, and m^2/s for position/velocity.
        //uint16_t covariance[36];
        /*for(int i=0;i<36;i++){
        	covariance[i] = canardConvertNativeFloatToFloat16( (float)i );
			canardEncodeScalar(buffer, offset, 16, &covariance[i]);
			offset+=16;
        }*/

        canardBroadcast(&g_canard,
        				UAVCAN_EQUIPMENT_GNSS_FIX2_SIGNATURE,
						UAVCAN_EQUIPMENT_GNSS_FIX2_ID,
                        &transfer_id,
                        CANARD_TRANSFER_PRIORITY_LOW,
                        &buffer[0],
						UAVCAN_EQUIPMENT_GNSS_FIX2_MESSAGE_SIZE);
}

void publishActuatorStatus(void){
	   static uint32_t publish_time = 0;
	    static int step = 0;
	    if(HAL_GetTick() < publish_time + PUBLISHER_PERIOD_mS) {return;} // rate limiting
	    publish_time = HAL_GetTick();

	    uint8_t buffer[UAVCAN_EQUIPMENT_ACTUATOR_STATUS_MESSAGE_SIZE];
	    memset(buffer,0x00,UAVCAN_EQUIPMENT_ACTUATOR_STATUS_MESSAGE_SIZE);

	    static uint8_t transfer_id = 0;

	    uint32_t offset = 0;

	    	uint8_t actuator_id = 1;
	    	canardEncodeScalar(buffer, offset, 8, &actuator_id);
	    	offset+=8;

	    	uint16_t position = canardConvertNativeFloatToFloat16(1);      //  16# meter or radian
	    	canardEncodeScalar(buffer, offset, 16, &position);
	    	offset+=16;

	    	uint16_t force = canardConvertNativeFloatToFloat16(2);        // 16  # Newton or Newton metre
	    	canardEncodeScalar(buffer, offset, 16, &force);
	    	offset+=16;

	    	uint16_t speed = canardConvertNativeFloatToFloat16(3);       // 16 # meter per second or radian per second
	    	canardEncodeScalar(buffer, offset, 16, &speed);
	    	offset+=16;


	    	offset+=1;//void1

	    	uint8_t power_rating_pct = 50;
	    	//uint7 power_rating_pct                # 0 - unloaded, 100 - full load
	    	canardEncodeScalar(buffer, offset, 7, &power_rating_pct);
	    	offset+=7;

	        canardBroadcast(&g_canard,
	        				UAVCAN_EQUIPMENT_ACTUATOR_STATUS_SIGNATURE,
							UAVCAN_EQUIPMENT_ACTUATOR_STATUS_ID,
	                        &transfer_id,
	                        CANARD_TRANSFER_PRIORITY_LOW,
	                        buffer,
							UAVCAN_EQUIPMENT_ACTUATOR_STATUS_MESSAGE_SIZE);
}

void publishMagneticFieldStrength2(void){
	   static uint32_t publish_time = 0;
	    static int step = 0;
	    if(HAL_GetTick() < publish_time + UAVCAN_EQUIPMENT_AHRS_MAG2_PERIOD_MS) {return;} // rate limiting
	    publish_time = HAL_GetTick();

	    uint8_t buffer[UAVCAN_EQUIPMENT_AHRS_MAG2_MESSAGE_SIZE];
	    memset(buffer,0x00,UAVCAN_EQUIPMENT_AHRS_MAG2_MESSAGE_SIZE);

	    static uint8_t transfer_id = 0;

	    uint32_t offset = 0;
/*
	    	uint8_t sensor_id = 0;
	    	canardEncodeScalar(buffer, offset, 8, &sensor_id);
	    	offset+=8;
*/
	    	uint16_t x = canardConvertNativeFloatToFloat16(HMC5883.mag[0]);      //  16# meter or radian
	    	canardEncodeScalar(buffer, offset, 16, &x);
	    	offset+=16;

	    	uint16_t y = canardConvertNativeFloatToFloat16(HMC5883.mag[1]);        // 16  # Newton or Newton metre
	    	canardEncodeScalar(buffer, offset, 16, &y);
	    	offset+=16;

	    	uint16_t z = canardConvertNativeFloatToFloat16(HMC5883.mag[2]);       // 16 # meter per second or radian per second
	    	canardEncodeScalar(buffer, offset, 16, &z);
	    	offset+=16;

	        canardBroadcast(&g_canard,
	        				UAVCAN_EQUIPMENT_AHRS_MAG2_SIGNATURE,
							UAVCAN_EQUIPMENT_AHRS_MAG2_ID,
	                        &transfer_id,
	                        CANARD_TRANSFER_PRIORITY_LOW,
	                        buffer,
							UAVCAN_EQUIPMENT_AHRS_MAG2_MESSAGE_SIZE);
}

void publishAirDataRaw(void){
	   static uint32_t publish_time = 0;
	    static int step = 0;
	    if(HAL_GetTick() < publish_time + UAVCAN_EQUIPMENT_AIR_DATA_RAW_PERIOD_MS) {return;} // rate limiting
	    publish_time = HAL_GetTick();

	    uint8_t buffer[UAVCAN_EQUIPMENT_AIR_DATA_RAW_MESSAGE_SIZE];
	    memset(buffer,0x00,UAVCAN_EQUIPMENT_AIR_DATA_RAW_MESSAGE_SIZE);

	    static uint8_t transfer_id = 0;

	    uint32_t offset = 0;

	    	uint8_t flag = 0;
	    	canardEncodeScalar(buffer, offset, 8, &flag);
	    	offset+=8;

	        float static_pressure = 0;
	        canardEncodeScalar(buffer, offset, 32, &static_pressure);
	        offset+=32;

	        float differential_pressure = airdata.diff_press_pa_raw;
	        canardEncodeScalar(buffer, offset, 32, &differential_pressure);
	        offset+=32;

	    	uint16_t x = canardConvertNativeFloatToFloat16( airdata.temperature+273.15 );
	    	canardEncodeScalar(buffer, offset, 16, &x);
	    	offset+=16;

	    	//uint16_t y = canardConvertNativeFloatToFloat16(HMC5883.mag[1]);        // 16  # Newton or Newton metre
	    	canardEncodeScalar(buffer, offset, 16, &x);
	    	offset+=16;

	    	//uint16_t  = canardConvertNativeFloatToFloat16(HMC5883.mag[2]);       // 16 # meter per second or radian per second
	    	canardEncodeScalar(buffer, offset, 16, &x);
	    	offset+=16;

	    	canardEncodeScalar(buffer, offset, 16, &x);
	    	offset+=16;
/*
	    	canardEncodeScalar(buffer, offset, 16, &x);
	    	offset+=16;
	    	canardEncodeScalar(buffer, offset, 16, &x);
	    	offset+=16;
	    	canardEncodeScalar(buffer, offset, 16, &x);
	    	offset+=16;
	    	canardEncodeScalar(buffer, offset, 16, &x);
	    	offset+=16;
	    	canardEncodeScalar(buffer, offset, 16, &x);
	    	offset+=16;
*/
	        canardBroadcast(&g_canard,
	        				UAVCAN_EQUIPMENT_AIR_DATA_RAW_SIGNATURE,
							UAVCAN_EQUIPMENT_AIR_DATA_RAW_ID,
	                        &transfer_id,
							CANARD_TRANSFER_PRIORITY_MEDIUM,
	                        buffer,
							UAVCAN_EQUIPMENT_AIR_DATA_RAW_MESSAGE_SIZE);
}

// https://legacy.uavcan.org/Specification/7._List_of_standard_data_types/#fueltankstatus
void fuelTankStatus(void)
{
	static uint32_t publish_time = 0;
	//static int step = 0;
	if(HAL_GetTick() < publish_time + PUBLISHER_PERIOD_mS) {return;} // rate limiting
	publish_time = HAL_GetTick();

    uint8_t buffer[UAVCAN_EQUIPMENT_ICE_FUELTANKSTATUS_MESSAGE_SIZE];
    memset(buffer,0x00,UAVCAN_EQUIPMENT_ICE_FUELTANKSTATUS_MESSAGE_SIZE);

    uint8_t nu = 0;
    uint8_t val = 22;
    float vol = 33;
    float rate = 12;
    float temp = 32;
    uint8_t id = 1;

    static uint8_t transfer_id = 0;
    canardEncodeScalar(buffer, 0, 		9, 	&nu);
    canardEncodeScalar(buffer, 9, 		7, 	&val);
    canardEncodeScalar(buffer, 16, 		32, &vol);
    canardEncodeScalar(buffer, 16+32, 	32, &rate);
    canardEncodeScalar(buffer, 16+64, 	16, &temp);
    canardEncodeScalar(buffer, 32+64, 	8, &id);
    //memcpy(&buffer[4], "sin", 3);
    canardBroadcast(&g_canard,
    				UAVCAN_EQUIPMENT_ICE_FUELTANKSTATUS_SIGNATURE,
					UAVCAN_EQUIPMENT_ICE_FUELTANKSTATUS_ID,
                    &transfer_id,
                    CANARD_TRANSFER_PRIORITY_LOW,
                    &buffer[0],
					UAVCAN_EQUIPMENT_ICE_FUELTANKSTATUS_MESSAGE_SIZE);
}


void makeNodeStatusMessage(uint8_t buffer[UAVCAN_NODE_STATUS_MESSAGE_SIZE])
{
    uint8_t node_health = UAVCAN_NODE_HEALTH_OK;
    uint8_t node_mode   = UAVCAN_NODE_MODE_OPERATIONAL;
    memset(buffer, 0, UAVCAN_NODE_STATUS_MESSAGE_SIZE);
    uint32_t uptime_sec = (HAL_GetTick() / 1000);
    canardEncodeScalar(buffer,  0, 32, &uptime_sec);
    canardEncodeScalar(buffer, 32,  2, &node_health);
    canardEncodeScalar(buffer, 34,  3, &node_mode);
}

uint16_t makeNodeInfoMessage(uint8_t buffer[UAVCAN_GET_NODE_INFO_RESPONSE_MAX_SIZE])
{
    memset(buffer, 0, UAVCAN_GET_NODE_INFO_RESPONSE_MAX_SIZE);
    makeNodeStatusMessage(buffer);
   
    buffer[7] = APP_VERSION_MAJOR;
    buffer[8] = APP_VERSION_MINOR;
    buffer[9] = 1;                          // Optional field flags, VCS commit is set
    uint32_t u32 = GIT_HASH;
    canardEncodeScalar(buffer, 80, 32, &u32); 
    
    readUniqueID(&buffer[24]);

    switch(parameters[17].val){
    case 0:
    	name = APP_NODE_NAME;
    	break;
    case 1:
    	name = 'right drone group';
    	break;
    case 2:
    	name = 'left drone group';
    	break;
    case 3:
    	name = 'right aileron';
    	break;
    case 4:
    	name = 'left aileron';
    	break;
    case 5:
    	name = 'tail group';
    	break;
    default:
    	name = 'sq.can';
    	break;
    }

    size_t name_len = sizeof(APP_NODE_NAME) ;
    memcpy(&buffer[41], APP_NODE_NAME, name_len);

    return 41 + name_len ;
}

void readUniqueID(uint8_t* out_uid)
{
    for (uint8_t i = 0; i < UNIQUE_ID_LENGTH_BYTES; i++)
    {
        out_uid[i] = i;
    }
}


void rawcmdHandleCanard(CanardRxTransfer* transfer)
{
    
    int offset = 0;
    for (int i = 0; i<10; i++)
    {
        if (canardDecodeScalar(transfer, offset, 14, true, &rc_pwm[i])<14) { break; }
        offset += 14;
    }
    pwmUpdate();
}

void showRcpwmonUart()
{
    char str[5];
    itoa(rc_pwm[0],str,10);
    HAL_UART_Transmit(&huart1,(uint8_t*)str,5,0xffff);
    HAL_UART_Transmit(&huart1,(uint8_t*)"\n",2,0xffff);
}

// update PWM out registers from acquiered data
void pwmUpdate(void)
{
	//A1
	uint32_t pwm = pwmMap(rc_pwm,parameters[1].val, parameters[5].val, parameters[9].val,parameters[13].val);//pwmMapping(rc_pwm,parameters[1].val, parameters[5].val);
	if( ((int32_t)pwm) >= 0 )
		test[0] = setPWM(A1, pwm );
	//A2
	pwm = pwmMap(rc_pwm,parameters[2].val, parameters[6].val, parameters[10].val, parameters[14].val);//pwmMapping(rc_pwm,parameters[2].val, parameters[6].val);
	if( ((int32_t)pwm) >= 0 )
		test[1] = setPWM(A2, pwm );
	//B1
	pwm = pwmMap(rc_pwm,parameters[3].val, parameters[7].val, parameters[11].val, parameters[15].val);//pwmMappingInv(rc_pwm,parameters[3].val, parameters[7].val);
	if( ((int32_t)pwm) >= 0 )
		test[2] = setPWM(B1, pwm );
	//B2
	pwm = pwmMap(rc_pwm,parameters[4].val, parameters[8].val, parameters[12].val, parameters[16].val);//pwmMapping(rc_pwm,parameters[4].val, parameters[8].val);
	if( ((int32_t)pwm) >= 0 )
		test[3] = setPWM(B2, pwm );
	/*int32_t rc_channel = parameters[3].val;
	if(rc_channel>=0){
		float pwm_in = (float)rc_pwm[ rc_channel ];
		pwm_in = pwm_in/8191.0*1000.0 + 1000.0; // mapping input from 1000 us to 2000 us
		uint32_t pwm = (uint32_t)pwm_in;
		setPWM(B1, pwm );
		//test = pwm;
	}*/
	count=0;
}
void goDefaultPWM(void)
{
	//uint32_t pwm = pwmMap(rc_pwm,parameters[1].val, parameters[5].val, parameters[9].val);//pwmMapping(rc_pwm,parameters[1].val, parameters[5].val);
	//if( ((int32_t)pwm) >= 0 )
		test[0] = setPWM(A1, parameters[13].val );
	//A2
	//pwm = pwmMap(rc_pwm,parameters[2].val, parameters[6].val, parameters[10].val);//pwmMapping(rc_pwm,parameters[2].val, parameters[6].val);
	//if( ((int32_t)pwm) >= 0 )
		test[1] = setPWM(A2, parameters[14].val );
	//B1
	//pwm = pwmMap(rc_pwm,parameters[3].val, parameters[7].val, parameters[11].val);//pwmMappingInv(rc_pwm,parameters[3].val, parameters[7].val);
	//if( ((int32_t)pwm) >= 0 )
		test[2] = setPWM(B1, parameters[15].val);
	//B2
	//pwm = pwmMap(rc_pwm,parameters[4].val, parameters[8].val, parameters[12].val);//pwmMapping(rc_pwm,parameters[4].val, parameters[8].val);
	//if( ((int32_t)pwm) >= 0 )
		test[3] = setPWM(B2, parameters[16].val);
}


param_t * getParamByIndex(uint16_t index)
{
  if(index >= ARRAY_SIZE(parameters)) 
  {
    return NULL;
  }

  return &parameters[index];
}

param_t * getParamByName(uint8_t * name)
{
  for(uint16_t i = 0; i < ARRAY_SIZE(parameters); i++)
  {
    if(strncmp((char const*)name, (char const*)parameters[i].name,strlen((char const*)parameters[i].name)) == 0) 
    {
      return &parameters[i];
    }
  }      
  return NULL;
}

uint16_t encodeParamCanard(param_t * p, uint8_t * buffer)
{
    uint8_t n     = 0;
    int offset    = 0;
    uint8_t tag   = 1;
    if(p==NULL)
    {   
        tag = 0;
        canardEncodeScalar(buffer, offset, 5, &n);
        offset += 5;
        canardEncodeScalar(buffer, offset,3, &tag);
        offset += 3;
        
        canardEncodeScalar(buffer, offset, 6, &n);
        offset += 6;
        canardEncodeScalar(buffer, offset,2, &tag);
        offset += 2;
        
        canardEncodeScalar(buffer, offset, 6, &n);
        offset += 6;
        canardEncodeScalar(buffer, offset, 2, &tag);
        offset += 2;
        buffer[offset / 8] = 0;
        return ( offset / 8 + 1 );
    }
    canardEncodeScalar(buffer, offset, 5,&n);
    offset += 5;
    canardEncodeScalar(buffer, offset, 3, &tag);
    offset += 3;
    canardEncodeScalar(buffer, offset, 64, &p->val);
    offset += 64;
    
    canardEncodeScalar(buffer, offset, 5, &n);
    offset += 5;
    canardEncodeScalar(buffer, offset, 3, &tag);
    offset += 3;
    canardEncodeScalar(buffer, offset, 64, &p->defval);
    offset += 64;
    
    canardEncodeScalar(buffer, offset, 6, &n);
    offset += 6;
    canardEncodeScalar(buffer, offset, 2, &tag);
    offset += 2;
    canardEncodeScalar(buffer, offset, 64, &p->max);
    offset += 64;
    
    canardEncodeScalar(buffer, offset, 6, &n);
    offset += 6;
    canardEncodeScalar(buffer, offset,2,&tag);
    offset += 2;
    canardEncodeScalar(buffer, offset,64,&p->min);
    offset += 64;
    
    memcpy(&buffer[offset / 8], p->name, strlen((char const*)p->name));
    return  (offset/8 + strlen((char const*)p->name)); 
}


void getsetHandleCanard(CanardRxTransfer* transfer)
{
    uint16_t index = 0xFFFF;
    uint8_t tag    = 0;
    int offset     = 0;
    int64_t val    = 0;

    canardDecodeScalar(transfer, offset,  13, false, &index);
    offset += 13;
    canardDecodeScalar(transfer, offset, 3, false, &tag);
    offset += 3;

    if(tag == 1)
    {
        canardDecodeScalar(transfer, offset, 64, false, &val);
        offset += 64;
    } 

    uint16_t n = transfer->payload_len - offset / 8 ;
    uint8_t name[16]      = "";
    for(int i = 0; i < n; i++)
    {
        canardDecodeScalar(transfer, offset, 8, false, &name[i]);
        offset += 8;
    }

    param_t * p = NULL;

    if(strlen((char const*)name))
    {
        p = getParamByName(name);
    }
    else
    {
        p = getParamByIndex(index);
    }

    if((p)&&(tag == 1))
    {
        p->val = val;

  //      HAL_FLASH_Unlock();
 //       HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, 0x0801FF00, 6);
  //      HAL_FLASH_Lock();

        /*flash_unlock();
        flash_erase();
        //flash_write((uint32_t)STORAGE_PAGE, parameters, 3);
        flash_write_param( (uint32_t)STORAGE_PAGE, parameters[0].val);
        flash_write_param( (uint32_t)STORAGE_PAGE+4, parameters[1].val);
        flash_write_param( (uint32_t)STORAGE_PAGE+4*2, parameters[2].val);
        flash_lock();*/
        //osSemaphoreRelease(myBinarySem01Handle);
    }

    uint8_t  buffer[64] = "";
    uint16_t len = encodeParamCanard(p, buffer);
    canardRequestOrRespond(&g_canard,
                           transfer->source_node_id,
                           UAVCAN_PROTOCOL_PARAM_GETSET_SIGNATURE,
                           UAVCAN_PROTOCOL_PARAM_GETSET_ID,
                           &transfer->transfer_id,
                           transfer->priority,
                           CanardResponse,
                           &buffer[0],
                           (uint16_t)len);

}

void saveHandle(CanardRxTransfer* transfer)
{
    //uint16_t index = 0xFFFF;
    //uint8_t tag    = 0;
    int offset     = 0;
    int64_t val    = 0;
    int8_t ok = 1;

    uint8_t opcode;

    canardDecodeScalar(transfer, offset,  8, false, &opcode);
    offset += 8;
    //canardDecodeScalar(transfer, offset, 3, false, &tag);
    //offset += 3;

    uint8_t  buffer[64] = "";
    //uint16_t len = encodeParamCanard(p, buffer);
    canardEncodeScalar(buffer, 0, 		48, 	&val);
    canardEncodeScalar(buffer, 48, 		1, 	&ok);

    canardRequestOrRespond(&g_canard,
                           transfer->source_node_id,
                           UAVCAN_PROTOCOL_PARAM_EXECUTEOPCODE_SIGNATURE,
                           UAVCAN_PROTOCOL_PARAM_EXECUTEOPCODE_ID,
                           &transfer->transfer_id,
                           transfer->priority,
                           CanardResponse,
                           &buffer[0],
                           7);

    //if(tag == 1)
    	//osSemaphoreRelease(myCountingSem01Handle);
    updateStorage();
}

void updateStorage(void){
	flash_unlock();
	        flash_erase();
	        //flash_write((uint32_t)STORAGE_PAGE, parameters, 3);
	for(int i = 0; i < param_num; i++)
	{
		flash_write_param( (int32_t)(STORAGE_PAGE+4*i), parameters[i].val);
	}
	        /*flash_write_param( (uint32_t)STORAGE_PAGE+4, parameters[1].val);
	        flash_write_param( (uint32_t)STORAGE_PAGE+4*2, parameters[2].val);*/
	        flash_lock();
}
