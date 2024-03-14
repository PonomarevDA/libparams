/*
 * Copyright (c) 2024 Dmitry Ponomarev <ponomarevda96@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Reference: https://www.st.com/resource/en/programming_manual/pm0075-
 * stm32f10xxx-flash-memory-microcontrollers-stmicroelectronics.pdf
 */

#ifndef PLATFORM_SPECIFIC_STM32F103TB_FLASH_REGISTERS_H_
#define PLATFORM_SPECIFIC_STM32F103TB_FLASH_REGISTERS_H_

// STM32F103TB constants
#define FLASH_PAGE_SIZE 1024

// Flash memory interface registers (medium-density devices)
typedef struct {
    volatile uint32_t ACR;          ///< 0x4002 2000 - 0x4002 2003
    volatile uint32_t KEYR;         ///< 0x4002 2004 - 0x4002 2007
    volatile uint32_t OPTKEYR;      ///< 0x4002 2008 - 0x4002 200B
    volatile uint32_t SR;           ///< 0x4002 200C - 0x4002 200F
    volatile uint32_t CR;           ///< 0x4002 2010 - 0x4002 2013
    volatile uint32_t AR;           ///< 0x4002 2014 - 0x4002 2017
    volatile uint32_t RESERVED;     ///< 0x4002 2018 - 0x4002 201B
    volatile uint32_t OBR;          ///< 0x4002 201C - 0x4002 201F
    volatile uint32_t WRPR;         ///< 0x4002 2020 - 0x4002 2023
} FlashMemoryInterfaceRegisters;

#define FLASH ((FlashMemoryInterfaceRegisters *)0x40022000UL)

// 3.2 FPEC key register (FLASH_KEYR)
#define FLASH_KEYR_KEY1         0x45670123UL
#define FLASH_KEYR_KEY2         0xCDEF89ABUL

// 3.4 Flash status register (FLASH_SR)
#define FLASH_SR_BSY            (1 << 0)    // This indicates that a Flash operation is in progress
#define FLASH_SR_PGERR          (1 << 2)    // Programming error
#define FLASH_SR_WRPRTERR       (1 << 4)    // Write protection error
#define FLASH_SR_EOP            (1 << 5)    // End of operation

// 3.5 Flash control register (FLASH_CR)
#define FLASH_CR_LOCK           (1 << 7)
#define FLASH_CR_PER            (1 << 1)
#define FLASH_CR_PG             (1 << 0)
#define FLASH_CR_STRT           (1 << 6)

// 3.7 Option byte register (FLASH_OBR)
#define FLASH_OBR_OPTERR        (1 << 0)

#endif  // PLATFORM_SPECIFIC_STM32F103TB_FLASH_REGISTERS_H_
