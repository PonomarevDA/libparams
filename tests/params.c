/*
 * Copyright (C) 2017 Konstantin Burdinov <sainquake@gmail.com>
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

/**
 * @file params.c
 * @author ko.burdinov, d.ponomarev
 * @date March 5, 2021
 */

#include <assert.h>
#include "storage.h"

/**
 * @note Names of these params should not contain spaces, because Mavlink console can't handle them
 */

IntegerCell_t __attribute__((weak)) parameters[] = {
    // name                         val     min     max     default
    {(uint8_t*)"identifier",        50,     0,      100,    50},
};

StringCell_t __attribute__((weak)) str_params[] = {
    // name                         val             default
    {(uint8_t*)"name",              "custom_name",  "default_name"},
};

static_assert(sizeof(parameters) + sizeof(str_params) < PAGE_SIZE_BYTES, "Params are out of mem.");
