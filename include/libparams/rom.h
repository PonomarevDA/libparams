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
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief This structure keeps all ROM driver related internal veriables.
 * Use rom->inited to verify is ROM driver has been initilized or not.
 * Use romInit() to create an instance.
 */
typedef struct {
    size_t addr;
    size_t first_page_idx;
    size_t total_size;
    size_t pages_amount;
    bool inited;
    bool erased;
    bool write_protected;
} RomDriverInstance;


/**
 * @brief In order to use ROM driver, you should create an instance by calling this function.
 * @param first_page Index of the first page. Negative values are counted from the end.
 * On stm32f103, -1 means the latest page (equal to idx=127).
 * On stm32g0, -1 means the latest page (equal idx=255).
 * On Ubuntu platform, it is safe to use -1 even if it has a single page (equal to idx=0).
 * @param pages_num The amount of allocated pages. At least 1 page is required.
 * @return ROM driver instance. It will have inited=true on success and inited=false on failure.
 * @note Example of a single latest page allocation: romInit(-1, 1)
 */
RomDriverInstance romInit(int32_t first_page_idx, size_t pages_amount);

/**
 * @brief Return the number of bytes read (may be less than requested_size).
 */
size_t romRead(const RomDriverInstance* rom,
               size_t                   offset,
               uint8_t*                 data,
               size_t                   requested_size);

/**
 * @return the number of bytes wrote on success. On failure returns a negative error code:
 * - LIBPARAMS_ROM_WRITE_BAD_ARGS_ERROR if arguments are incorrect
 * - LIBPARAMS_ROM_WRITE_PROTECTED_ERROR if romBeginWrite has not been called
 * - Any flash driver or HAL related errors
 */
void romBeginWrite(RomDriverInstance* rom);
int32_t romWrite(const RomDriverInstance*    rom,
                 size_t                      offset,
                 const uint8_t*              data,
                 size_t                      size);
void romEndWrite(RomDriverInstance*   rom);

uint32_t romGetAvailableMemory(const RomDriverInstance* rom);

int32_t romGetErrorCode();

#ifdef __cplusplus
}
#endif

#endif  // LIBPARAM_ROM_H_
