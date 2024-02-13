/*
 * Copyright (c) 2022-2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef LIBPARAM_ROM_H_
#define LIBPARAM_ROM_H_

#include <stdint.h>
#include <stddef.h>

#define PARAM_STRING_MAX_SIZE   20

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    size_t addr;
    size_t start_page_idx;
    size_t size_bytes;
    size_t size_pages;
} RomDriver;

/**
 * @brief By default it is initialized as last page only.
 * @return 0 if success, otherwise < 0
 */
int8_t romInit(size_t first_page_idx, size_t pages_amount);

/**
 * @brief Return the number of bytes read (may be less than size).
 */
size_t romRead(size_t offset, uint8_t* data, size_t size);

/**
 * @brief Return the number of bytes wrote (0 in case of error).
 */
void romBeginWrite();
size_t romWrite(size_t offset, const uint8_t* data, size_t size);
void romEndWrite();

uint32_t romGetAvailableMemory();

#ifdef __cplusplus
}
#endif

#endif  // LIBPARAM_ROM_H_
