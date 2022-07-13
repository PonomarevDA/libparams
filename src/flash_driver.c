/**
 * @file flash_driver.c
 * @author ko.burdinov, d.ponomarev
 * @date Apr 1, 2020
 */

#include "flash_driver.h"
#include "stm32f1xx_hal.h"


void flashUnlock() {
    HAL_FLASH_Unlock();
}
void flashLock() {
    HAL_FLASH_Lock();
}
void flashErase(uint32_t page_address, uint32_t num_pf_pages) {
    FLASH_EraseInitTypeDef FLASH_EraseInitStruct = {
        .TypeErase = FLASH_TYPEERASE_PAGES,
        .PageAddress = (uint32_t)page_address,
        .NbPages = num_pf_pages
    };
    uint32_t error = 0;
    HAL_FLASHEx_Erase(&FLASH_EraseInitStruct, &error);
}

int8_t flashWriteWord(uint32_t address, uint32_t data) {
    HAL_StatusTypeDef hal_status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address, data);
    return (hal_status != HAL_OK) ? -1 : 0;
}
