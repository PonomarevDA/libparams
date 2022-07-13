/**
 * @file flash.c
 * @author ko.burdinov, d.ponomarev
 * @date Apr 1, 2020
 */

#include "flash.h"
#include <stddef.h>
#include <string.h>
#include "flash_driver.h"

#ifndef MCU_TYPE
    #define STM32F103_128KB
#endif
#ifdef STM32F103_128KB
    #define FLASH_START_ADDR            0x08000000
    #define FLASH_SIZE_KBYTES           128 * 1024
    #define FLASH_NUM_OF_PAGES          128
    #define FLASH_LAST_PAGE_ADDR        0x0801FC00
    #define FLASH_END_ADDR              0x0801FFFF
#endif

#define STORAGE_PAGE                    0x0801FC00


///< Default values correspond to the last page access only.
static const volatile int32_t *flash_memory_ptr = (const volatile int32_t *)FLASH_LAST_PAGE_ADDR;
static size_t flash_size = PAGE_SIZE_BYTES;
static size_t num_of_pages = 1;


int8_t flashInit(uint8_t first_page_idx, uint8_t pages_amount) {
    size_t last_page_num = first_page_idx + pages_amount;
    if (last_page_num > FLASH_NUM_OF_PAGES || pages_amount == 0) {
        return -1;
    }

    flash_memory_ptr = (const volatile int32_t *)(FLASH_START_ADDR + first_page_idx * PAGE_SIZE_BYTES);
    flash_size = pages_amount * PAGE_SIZE_BYTES;
    num_of_pages = pages_amount;
    return 0;
}
size_t flashRead(size_t offset, uint8_t* data, size_t requested_size) {
    if (data == NULL || offset >= flash_size || requested_size == 0) {
        return 0;
    }

    size_t allowed_size = flash_size - offset;
    size_t bytes_to_read = (allowed_size < requested_size) ? allowed_size : requested_size;
    memcpy(data, ((const uint8_t*)flash_memory_ptr) + offset, bytes_to_read);
    return bytes_to_read;
}
size_t flashWrite(size_t offset, const uint8_t* data, size_t size) {
    if (data == NULL || offset >= flash_size || size == 0 || offset + size > flash_size ) {
        return 0;
    }

    flashUnlock();
    flashErase((uint32_t)flash_memory_ptr, num_of_pages);
    uint32_t address = (uint32_t)flash_memory_ptr + offset;
    uint32_t written_data = *(const uint32_t*) (void*) data;
    int8_t status = flashWriteWord(address, written_data);
    flashLock();

    return (status != -1) ? size : 0;
}


///< deprecated???
int8_t flashWriteU32ByIndex(uint8_t param_idx, uint32_t data) {
    return flashWriteWord((int32_t)(flash_memory_ptr + 4 * param_idx), data);
}
int64_t flashReadI32ByIndex(uint8_t param_idx) {
    int32_t param_value = flash_memory_ptr[param_idx];
    return param_value;
}
