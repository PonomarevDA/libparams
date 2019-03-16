/*
 * Copyright (C) 2017 Konstantin Burdinov <sainquake@gmail.com>
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

/**
 * @file esc.c
 * @author sainquake
 */

#ifndef INC_SQ_UAVCAN_H_
#define INC_SQ_UAVCAN_H_

#include "canard.h"
#include "canard_stm32.h"

#include "stm32f1xx_hal.h"
#include "main.h"

#include "sq_flash.h"
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

#define UAVCAN_PROTOCOL_PARAM_GETSET_ID                             11
#define UAVCAN_PROTOCOL_PARAM_GETSET_SIGNATURE                      0xa7b622f939d1a4d5    


#define UAVCAN_PROTOCOL_PARAM_EXECUTEOPCODE_ID                      10
#define UAVCAN_PROTOCOL_PARAM_EXECUTEOPCODE_SIGNATURE               0xa7b622f939d1a466

void uavcanInit(void);

#endif /* INC_SQ_UAVCAN_H_ */
