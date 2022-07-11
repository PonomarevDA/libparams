/**
 * @file flash.c
 * @author ko.burdinov, d.ponomarev
 * @date Apr 1, 2020
 */

#include "flash.h"
#include <stddef.h>
#include <string.h>
#include "stm32f1xx_hal.h"

#ifndef MCU_TYPE
    #define STM32F103_128KB
#endif
#ifdef STM32F103_128KB
    #define FLASH_START_ADDR            0x08000000
    #define FLASH_SIZE_KBYTES           128 * 1024
    #define FLASH_NUM_OF_PAGES          128
    #define FLASH_PAGE_SIZE_BYTES       1024
    #define FLASH_LAST_PAGE_ADDR        0x0801FC00
    #define FLASH_END_ADDR              0x0801FFFF
#endif

#define STORAGE_PAGE                    0x0801FC00


///< Default values correspond to the last page access only.
static const volatile int32_t *flash_memory_ptr = (const volatile int32_t *)FLASH_LAST_PAGE_ADDR;
static size_t flash_size = FLASH_PAGE_SIZE_BYTES;
static size_t num_of_pages = 1;


static int8_t flashWriteU32ByAddress(uint32_t address, uint32_t data);


int8_t flashInit(size_t initial_address, size_t size) {
    if ((initial_address % FLASH_PAGE_SIZE_BYTES != 0) ||
            initial_address < FLASH_START_ADDR ||
            initial_address > FLASH_LAST_PAGE_ADDR ||
            initial_address + size > FLASH_END_ADDR ||
            size == 0) {
        return -1;
    }
    flash_memory_ptr = (const volatile int32_t*)initial_address;
    flash_size = size;
    num_of_pages = 1 + (size - 1) / FLASH_PAGE_SIZE_BYTES;
    return 0;
}
size_t flashRead(size_t offset, uint8_t* data, size_t requested_size) {
    if (data == NULL || offset >= flash_size || requested_size == 0) {
        return 0;
    }
    size_t allowed_size = flash_size - offset;
    size_t bytes_to_read = (allowed_size < requested_size) ? allowed_size : requested_size;
    memcpy(data, (const void*)&flash_memory_ptr[offset], bytes_to_read);
    return bytes_to_read;
}
size_t flashWrite(size_t offset, const uint8_t* data, size_t size) {
    if (data == NULL || offset >= flash_size || size == 0) {
        return 0;
    }
    return 0;
}


void flashUnlock() {
    HAL_FLASH_Unlock();
}
void flashLock() {
    HAL_FLASH_Lock();
}
void flashErase() {
    FLASH_EraseInitTypeDef FLASH_EraseInitStruct = {
        .TypeErase = FLASH_TYPEERASE_PAGES,
        .PageAddress = (uint32_t)flash_memory_ptr,
        .NbPages = num_of_pages
    };
    uint32_t error = 0;
    HAL_FLASHEx_Erase(&FLASH_EraseInitStruct, &error);
}

int8_t flashWriteU32ByIndex(uint8_t param_idx, uint32_t data) {
    return flashWriteU32ByAddress((int32_t)(flash_memory_ptr + 4 * param_idx), data);
}
int64_t flashReadI32ByIndex(uint8_t param_idx) {
    int32_t param_value = flash_memory_ptr[param_idx];
    return param_value;
}

int8_t flashWriteU32ByAddress(uint32_t address, uint32_t data) {
    HAL_StatusTypeDef hal_status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address, data);
    return (hal_status != HAL_OK) ? -1 : 0;
}
