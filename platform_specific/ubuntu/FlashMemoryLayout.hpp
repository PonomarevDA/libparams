/*
 * Copyright (c) 2024 Anastasiia Stepanova <asiiapine@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef LIBPARAM_FLASH_MEMORY_LAYOUT_HPP_
#define LIBPARAM_FLASH_MEMORY_LAYOUT_HPP_

#include "storage.h"

typedef struct {
    IntegerDesc_t* integer_desc_pool;
    StringDesc_t* string_desc_pool;

    uint8_t num_int_params;
    uint8_t num_str_params;
} ParametersLayout_t;

typedef struct {
    uint8_t* memory_ptr;
    uint16_t page_size;
    uint8_t num_pages;
    uint32_t flash_size = page_size * num_pages;
} FlashMemoryLayout_t;

#endif  // LIBPARAM_FLASH_MEMORY_LAYOUT_HPP
