/*
 * Copyright (C) 2017 Konstantin Burdinov <sainquake@gmail.com>
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef INC_SQ_UAVCAN_H_
#define INC_SQ_UAVCAN_H_

#include "canard.h"
#include "canard_stm32.h"

#include "stm32f1xx_hal.h"
#include "main.h"

#include "sq_flash.h"
#include "sq_pwm.h"

#include "cmsis_os.h"

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))

#define CAN_SPEED                                                   1000000

#define NODE_ID                                                     50
#define APP_VERSION_MAJOR                                           0
#define APP_VERSION_MINOR                                           1
#define APP_NODE_NAME                                               "sq.can"
#define GIT_HASH                                                    0xBADC0FFE

#define UAVCAN_GET_NODE_INFO_DATA_TYPE_SIGNATURE                    0xee468a8121c46a9e
#define UAVCAN_GET_NODE_INFO_DATA_TYPE_ID                           1
#define UAVCAN_GET_NODE_INFO_RESPONSE_MAX_SIZE                      ((3015 + 7) / 8)

#define UAVCAN_EQUIPMENT_ESC_RAWCOMMAND_ID                          1030
#define UAVCAN_EQUIPMENT_ESC_RAWCOMMAND_SIGNATURE                   0x217f5c87d7ec951d
#define UAVCAN_EQUIPMENT_ESC_RAWCOMMAND_MAX_VALUE                   8192

#define UNIQUE_ID_LENGTH_BYTES                                      16

#define UAVCAN_NODE_HEALTH_OK                                       0
#define UAVCAN_NODE_HEALTH_WARNING                                  1
#define UAVCAN_NODE_HEALTH_ERROR                                    2
#define UAVCAN_NODE_HEALTH_CRITICAL                                 3

#define UAVCAN_NODE_MODE_OPERATIONAL                                0
#define UAVCAN_NODE_MODE_INITIALIZATION                             1

#define UAVCAN_NODE_STATUS_MESSAGE_SIZE                             7

#define UAVCAN_NODE_STATUS_MESSAGE_SIZE                             7
#define UAVCAN_NODE_STATUS_DATA_TYPE_ID                             341
#define UAVCAN_NODE_STATUS_DATA_TYPE_SIGNATURE                      0x0f0868d0c1a7c6f1

#define UAVCAN_PROTOCOL_DEBUG_KEYVALUE_ID                           16370   
#define UAVCAN_PROTOCOL_DEBUG_KEYVALUE_SIGNATURE                    0xe02f25d6e0c98000
#define UAVCAN_PROTOCOL_DEBUG_KEYVALUE_MESSAGE_SIZE                 62                   
#define UNIQUE_ID_LENGTH_BYTES                                      16

// Full name: uavcan.equipment.gnss.Fix
// Default data type ID: 1060
#define UAVCAN_EQUIPMENT_GNSS_FIX_ID                      1060
#define UAVCAN_EQUIPMENT_GNSS_FIX_SIGNATURE               0x54c1572b9e07f297
#define UAVCAN_EQUIPMENT_GNSS_FIX_MESSAGE_SIZE            42 //14+

// uavcan.equipment.gnss.Fix2
// Default data type ID: 1063
#define UAVCAN_EQUIPMENT_GNSS_FIX2_ID                      1063
#define UAVCAN_EQUIPMENT_GNSS_FIX2_SIGNATURE               0xca41e7000f37435f
#define UAVCAN_EQUIPMENT_GNSS_FIX2_MESSAGE_SIZE            177 //14+

//Full name: uavcan.equipment.ahrs.MagneticFieldStrength2
//Default data type ID: 1002
#define UAVCAN_EQUIPMENT_AHRS_MAG2_ID                      1001
#define UAVCAN_EQUIPMENT_AHRS_MAG2_SIGNATURE               0xb6ac0c442430297e
#define UAVCAN_EQUIPMENT_AHRS_MAG2_MESSAGE_SIZE            6 //14+
#define UAVCAN_EQUIPMENT_AHRS_MAG2_PERIOD_MS               330 //14+

//Full name: uavcan.equipment.actuator.Status
//Default data type ID: 1011

#define UAVCAN_EQUIPMENT_ACTUATOR_STATUS_ID                      1011
#define UAVCAN_EQUIPMENT_ACTUATOR_STATUS_SIGNATURE               0x5e9bba44faf1ea04
#define UAVCAN_EQUIPMENT_ACTUATOR_STATUS_MESSAGE_SIZE            8

//Full name: uavcan.equipment.air_data.RawAirData
//Default data type ID: 1027
#define UAVCAN_EQUIPMENT_AIR_DATA_RAW_ID                      1027
#define UAVCAN_EQUIPMENT_AIR_DATA_RAW_SIGNATURE               0xc77df38ba122f5da
#define UAVCAN_EQUIPMENT_AIR_DATA_RAW_MESSAGE_SIZE            17
#define UAVCAN_EQUIPMENT_AIR_DATA_RAW_PERIOD_MS               50

// uavcan.equipment.ice.FuelTankStatus
#define UAVCAN_EQUIPMENT_ICE_FUELTANKSTATUS_ID                      1129
#define UAVCAN_EQUIPMENT_ICE_FUELTANKSTATUS_SIGNATURE               0x286b4a387ba84bc4
#define UAVCAN_EQUIPMENT_ICE_FUELTANKSTATUS_MESSAGE_SIZE            13

#define UAVCAN_PROTOCOL_PARAM_GETSET_ID                             11
#define UAVCAN_PROTOCOL_PARAM_GETSET_SIGNATURE                      0xa7b622f939d1a4d5    

// uavcan.protocol.param.ExecuteOpcode
#define UAVCAN_PROTOCOL_PARAM_EXECUTEOPCODE_ID                      10
#define UAVCAN_PROTOCOL_PARAM_EXECUTEOPCODE_SIGNATURE               0xa7b622f939d1a466

extern UART_HandleTypeDef huart1;

extern uint32_t count;

extern osSemaphoreId_t myCountingSem01Handle;

extern float T,D;
typedef struct
{
uint8_t* name;
int64_t val; 
int64_t min;
int64_t max;
int64_t defval;
} param_t;

/*
struct _param_t{
uint8_t* name;
int64_t val;
int64_t min;
int64_t max;
int64_t defval;
};

typedef struct _param_t param_t;
*/

void showRcpwmonUart(void);

void sendCanard(void);

void receiveCanard(void);

void spinCanard(void);

void publishCanard(void);

void publishFix2(void);

void publishMagneticFieldStrength2(void);

void publishAirDataRaw(void);

void publishActuatorStatus(void);

void fuelTankStatus(void);

void showRcpwmonUart(void);

void rawcmdHandleCanard(CanardRxTransfer* transfer);

void getsetHandleCanard(CanardRxTransfer* transfer);

void getNodeInfoHandleCanard(CanardRxTransfer* transfer);

uint16_t makeNodeInfoMessage(uint8_t buffer[UAVCAN_GET_NODE_INFO_RESPONSE_MAX_SIZE]);

void uavcanInit(void);

void saveHandle(CanardRxTransfer* transfer);

void updateStorage(void);

void pwmUpdate(void);

void goDefaultPWM(void);

static const uint8_t  sine_wave[256] = 
{
  0x80, 0x83, 0x86, 0x89, 0x8C, 0x90, 0x93, 0x96,
  0x99, 0x9C, 0x9F, 0xA2, 0xA5, 0xA8, 0xAB, 0xAE,
  0xB1, 0xB3, 0xB6, 0xB9, 0xBC, 0xBF, 0xC1, 0xC4,
  0xC7, 0xC9, 0xCC, 0xCE, 0xD1, 0xD3, 0xD5, 0xD8,
  0xDA, 0xDC, 0xDE, 0xE0, 0xE2, 0xE4, 0xE6, 0xE8,
  0xEA, 0xEB, 0xED, 0xEF, 0xF0, 0xF1, 0xF3, 0xF4,
  0xF5, 0xF6, 0xF8, 0xF9, 0xFA, 0xFA, 0xFB, 0xFC,
  0xFD, 0xFD, 0xFE, 0xFE, 0xFE, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xFE, 0xFE, 0xFD,
  0xFD, 0xFC, 0xFB, 0xFA, 0xFA, 0xF9, 0xF8, 0xF6,
  0xF5, 0xF4, 0xF3, 0xF1, 0xF0, 0xEF, 0xED, 0xEB,
  0xEA, 0xE8, 0xE6, 0xE4, 0xE2, 0xE0, 0xDE, 0xDC,
  0xDA, 0xD8, 0xD5, 0xD3, 0xD1, 0xCE, 0xCC, 0xC9,
  0xC7, 0xC4, 0xC1, 0xBF, 0xBC, 0xB9, 0xB6, 0xB3,
  0xB1, 0xAE, 0xAB, 0xA8, 0xA5, 0xA2, 0x9F, 0x9C,
  0x99, 0x96, 0x93, 0x90, 0x8C, 0x89, 0x86, 0x83,
  0x80, 0x7D, 0x7A, 0x77, 0x74, 0x70, 0x6D, 0x6A,
  0x67, 0x64, 0x61, 0x5E, 0x5B, 0x58, 0x55, 0x52,
  0x4F, 0x4D, 0x4A, 0x47, 0x44, 0x41, 0x3F, 0x3C,
  0x39, 0x37, 0x34, 0x32, 0x2F, 0x2D, 0x2B, 0x28,
  0x26, 0x24, 0x22, 0x20, 0x1E, 0x1C, 0x1A, 0x18,
  0x16, 0x15, 0x13, 0x11, 0x10, 0x0F, 0x0D, 0x0C,
  0x0B, 0x0A, 0x08, 0x07, 0x06, 0x06, 0x05, 0x04,
  0x03, 0x03, 0x02, 0x02, 0x02, 0x01, 0x01, 0x01,
  0x01, 0x01, 0x01, 0x01, 0x02, 0x02, 0x02, 0x03,
  0x03, 0x04, 0x05, 0x06, 0x06, 0x07, 0x08, 0x0A,
  0x0B, 0x0C, 0x0D, 0x0F, 0x10, 0x11, 0x13, 0x15,
  0x16, 0x18, 0x1A, 0x1C, 0x1E, 0x20, 0x22, 0x24,
  0x26, 0x28, 0x2B, 0x2D, 0x2F, 0x32, 0x34, 0x37,
  0x39, 0x3C, 0x3F, 0x41, 0x44, 0x47, 0x4A, 0x4D,
  0x4F, 0x52, 0x55, 0x58, 0x5B, 0x5E, 0x61, 0x64,
  0x67, 0x6A, 0x6D, 0x70, 0x74, 0x77, 0x7A, 0x7D
};

#endif /* INC_SQ_UAVCAN_H_ */
