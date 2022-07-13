/**
 * @file flash_driver.h
 * @author d.ponomarev
 * @date Jul 12, 2022
 */

#ifndef LIBPARAM_FLASH_DRIVER_H_
#define LIBPARAM_FLASH_DRIVER_H_

#include <stdint.h>

void flashUnlock();
void flashLock();
void flashErase(uint32_t page_address, uint32_t num_pf_pages);
int8_t flashWriteWord(uint32_t address, uint32_t data);

#endif  // LIBPARAM_FLASH_DRIVER_H_
