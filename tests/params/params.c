/*
 * Copyright (c) 2022-2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <params.hpp>
#include <assert.h>
#include "storage.h"
#include "flash_driver.h"

IntegerDesc_t integer_desc_pool[512] = {
    {"uavcan.node.id",        0,      127,     50,        MUTABLE},
    {"uavcan.pub.mag.id",     0,      65535,   65535,     MUTABLE},
    {"uavcan.can.baudrate",   100000, 8000000, 1000000,   IMMUTABLE},
};
IntegerParamValue_t integer_values_pool[512];

StringDesc_t string_desc_pool[512] = {
    {"name", "Unknown", MUTABLE},
    {"uavcan.pub.mag.type", "uavcan.si.sample.magnetic_field_strength.Vector3", IMMUTABLE},
};
StringParamValue_t string_values_pool[512];
