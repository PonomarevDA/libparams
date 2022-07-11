/**
 * @file flash.c
 * @author ko.burdinov, d.ponomarev
 * @date Apr 1, 2020
 */

#include "flash.h"
#include <stddef.h>
#include "stm32f1xx_hal.h"

#define STORAGE_PAGE                    0x0801FC00
#define NUMBER_OF_PAGES_FOR_STORAGE     1

static const volatile int32_t *flash_memory_ptr = (const volatile int32_t *)STORAGE_PAGE;

static int8_t flashWriteU32ByAddress(uint32_t address, uint32_t data);


void flashUnlock() {
    HAL_FLASH_Unlock();
}
void flashLock() {
    HAL_FLASH_Lock();
}
void flashErase() {
    FLASH_EraseInitTypeDef FLASH_EraseInitStruct = {
        .TypeErase = FLASH_TYPEERASE_PAGES,
        .PageAddress = (uint32_t)STORAGE_PAGE,
        .NbPages = NUMBER_OF_PAGES_FOR_STORAGE
    };
    uint32_t error = 0;
    HAL_FLASHEx_Erase(&FLASH_EraseInitStruct, &error);
}

int8_t flashWriteU32ByIndex(uint8_t param_idx, uint32_t data) {
    return flashWriteU32ByAddress((int32_t)(STORAGE_PAGE + 4 * param_idx), data);
}
int64_t flashReadI32ByIndex(uint8_t param_idx) {
    int32_t param_value = flash_memory_ptr[param_idx];
    return param_value;
}
void flashReadStringByIndex(uint8_t param_idx) {
}

int8_t flashWriteStringByIndex(uint8_t param_idx, uint8_t* string) {
    const int32_t FIRST_ADDR = STORAGE_PAGE + PAGE_SIZE_BYTES - PARAM_STRING_MAX_SIZE;
    int32_t base_addr = FIRST_ADDR - PARAM_STRING_MAX_SIZE * param_idx;
    for (size_t word_idx = 0; word_idx < PARAM_STRING_MAX_SIZE/4; word_idx++) {
        uint32_t addr = base_addr + 4 * word_idx;
        flashWriteU32ByAddress(addr, *(uint32_t*)(&string[word_idx]));
    }
    return 0;
}

int8_t flashWriteU32ByAddress(uint32_t address, uint32_t data) {
    HAL_StatusTypeDef hal_status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address, data);
    return (hal_status != HAL_OK) ? -1 : 0;
}
