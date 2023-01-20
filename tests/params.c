/*
 * Copyright (C) 2020-2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
 * Copyright (C) 2020-2023 Raccoon Lab team
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <assert.h>
#include "storage.h"
#include "flash_driver.h"

/**
 * @note Names of these params should not contain spaces, because Mavlink console can't handle them
 */

// name                             min     max     default
IntegerDesc_t __attribute__((weak)) integer_desc_pool[] = {
    {(uint8_t*)"identifier",        0,      100,    50},
};
IntegerParamValue_t integer_values_pool[sizeof(integer_desc_pool) / sizeof(IntegerDesc_t)];

//   name                           default
StringDesc_t __attribute__((weak)) string_desc_pool[] = {
    {(uint8_t*)"name",              "default_name"},
};
StringParamValue_t string_values_pool[sizeof(string_desc_pool) / sizeof(StringDesc_t)];

static_assert(sizeof(integer_desc_pool) + sizeof(string_desc_pool) < PAGE_SIZE_BYTES, "Params are out of mem.");
