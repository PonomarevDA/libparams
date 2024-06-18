/*
 * Copyright (c) 2024 Stepanova Anastasiia <asiiapine@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "storage.h"
#ifndef LIBPARAM_PARAMETERS_HPP_
#define LIBPARAM_PARAMETERS_HPP_

enum IntParamsIndexes {
    PARAM_UAVCAN_NODE_ID,
    PARAM_MAG_ID,
    PARAM_CAN_BAUDRATE,
    PARAM_NAME,
    PARAM_MAG_TYPE,

    INTEGER_PARAMS_AMOUNT
};
#define NUM_OF_STR_PARAMS 2
#endif  // LIBPARAM_PARAMETERS_HPP_
