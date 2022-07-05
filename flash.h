/**
 * @file flash.h
 * @author ko.burdinov, d.ponomarev
 * @date Apr 1, 2020
 */

#ifndef LIBPARAM_FLASH_H_
#define LIBPARAM_FLASH_H_

#include <stdint.h>

#define PAGE_SIZE_BYTES         (1024)
#define PARAM_STRING_MAX_SIZE   20

/**
 * @brief You can read param value from flash memory at any time
 */
int64_t flashReadI32ByIndex(uint8_t param_idx);


/**
 * @brief To write a param value, you need initially to unlock the flash memory, then erase
 * desired page, write data and then lock it back
 */
void flashUnlock();
void flashErase();
int8_t flashWriteU32ByIndex(uint8_t param_idx, uint32_t data);
int8_t flashWriteStringByIndex(uint8_t param_idx, uint8_t* string);
void flashLock();

#endif  // LIBPARAM_FLASH_H_
