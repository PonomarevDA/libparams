/*
 * Copyright (c) 2026 Ilia Kliantsevich <iliawork112005@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "flash_driver.h"
#include "platform_flash_driver.h"
#include <string.h>
#include "libparams_error_codes.h"
#include "main.h"
#include "spi.h"

#define FM25V02_SIZE_BYTES         (32U * 1024U)
#define SPIFRAM_TRANSFER_TIMEOUT   100U
#define SPIFRAM_ERASE_CHUNK_SIZE   64U

#ifndef LIBPARAMS_SPIFRAM_SPI
    #define LIBPARAMS_SPIFRAM_SPI hspi5
#endif

#ifndef LIBPARAMS_SPIFRAM_CS_PORT
    #define LIBPARAMS_SPIFRAM_CS_PORT SPI5_NCS1_FRAM_GPIO_Port
#endif

#ifndef LIBPARAMS_SPIFRAM_CS_PIN
    #define LIBPARAMS_SPIFRAM_CS_PIN SPI5_NCS1_FRAM_Pin
#endif

static void flashInit(void);
static int8_t flashErase(uint32_t start_page_idx, uint32_t num_of_pages);
static int32_t flashWrite(const uint8_t* data, size_t offset, size_t bytes_to_write);
static int8_t flashUnlock(void);
static int8_t flashLock(void);
static size_t flashRead(uint8_t* data, size_t offset, size_t bytes_to_read);
static uint16_t flashGetNumberOfPages(void);
static uint32_t flashGetPageSize(void);

typedef enum {
    SPIFRAM_CMD_WREN  = 0x06U,  // Set Write Enable Latch
    SPIFRAM_CMD_WRDI  = 0x04U,  // Write Disable
    SPIFRAM_CMD_RDSR  = 0x05U,  // Read Status Register
    SPIFRAM_CMD_WRSR  = 0x01U,  // Write Status Register
    SPIFRAM_CMD_READ  = 0x03U,  // Read Memory Data
    SPIFRAM_CMD_FSTRD = 0x0BU,  // Fast Read Memory Data
    SPIFRAM_CMD_WRITE = 0x02U,  // Write Memory Data
    SPIFRAM_CMD_SLEEP = 0xB9U,  // Enter Sleep Mode
    SPIFRAM_CMD_RDID  = 0x9FU,  // Read Device ID
    SPIFRAM_CMD_SNR   = 0xC3U,  // Read Serial Number
} SpiframCommand;


static int8_t spiframNormalizeOffset(size_t offset, uint16_t* fram_offset);
static int8_t spiframWriteEnable(void);
static int8_t spiframWrite(uint16_t address, const uint8_t* data, uint16_t size);
static int8_t spiframRead(uint16_t address, uint8_t* data, uint16_t size);
static int8_t spiframTransmit(const uint8_t* data, uint16_t size);
static int8_t spiframReceive(uint8_t* data, uint16_t size);
static void spiframSelect(void) {
    HAL_GPIO_WritePin(LIBPARAMS_SPIFRAM_CS_PORT, LIBPARAMS_SPIFRAM_CS_PIN, GPIO_PIN_RESET);
}

static void spiframDeselect(void) {
    HAL_GPIO_WritePin(LIBPARAMS_SPIFRAM_CS_PORT, LIBPARAMS_SPIFRAM_CS_PIN, GPIO_PIN_SET);
}

static int8_t spiframTransmit(const uint8_t* data, uint16_t size) {
    if (data == NULL || size == 0U) {
        return LIBPARAMS_WRONG_ARGS;
    }
    HAL_StatusTypeDef status = HAL_SPI_Transmit(&LIBPARAMS_SPIFRAM_SPI,
                                                (uint8_t*)data,
                                                size,
                                                SPIFRAM_TRANSFER_TIMEOUT);
    return (status == HAL_OK) ? LIBPARAMS_OK : LIBPARAMS_UNKNOWN_HAL_ERROR;
}

static int8_t spiframReceive(uint8_t* data, uint16_t size) {
    if (data == NULL || size == 0U) {
        return LIBPARAMS_WRONG_ARGS;
    }
    HAL_StatusTypeDef status = HAL_SPI_Receive(&LIBPARAMS_SPIFRAM_SPI,
                                               data,
                                               size,
                                               SPIFRAM_TRANSFER_TIMEOUT);
    return (status == HAL_OK) ? LIBPARAMS_OK : LIBPARAMS_UNKNOWN_HAL_ERROR;
}

static int8_t spiframNormalizeOffset(size_t offset, uint16_t* fram_offset) {
    if (fram_offset == NULL) {
        return LIBPARAMS_WRONG_ARGS;
    }

    if (offset >= FLASH_START_ADDR) {
        offset -= FLASH_START_ADDR;
    }

    if (offset >= FM25V02_SIZE_BYTES) {
        return LIBPARAMS_WRONG_ARGS;
    }

    *fram_offset = (uint16_t)offset;
    return LIBPARAMS_OK;
}

static int8_t spiframWriteEnable(void) {
    const uint8_t cmd = SPIFRAM_CMD_WREN;

    spiframSelect();
    int8_t res = spiframTransmit(&cmd, sizeof(cmd));
    spiframDeselect();

    return res;
}

static int8_t spiframWrite(uint16_t address, const uint8_t* data, uint16_t size) {
    if (data == NULL || size == 0U || (uint32_t)address + size > FM25V02_SIZE_BYTES) {
        return LIBPARAMS_WRONG_ARGS;
    }

    int8_t res = spiframWriteEnable();
    if (res != LIBPARAMS_OK) {
        return res;
    }

    const uint8_t cmd_write[3] = {
        SPIFRAM_CMD_WRITE,
        (uint8_t)(address >> 8U),
        (uint8_t)(address & 0xFFU),
    };

    spiframSelect();
    res = spiframTransmit(cmd_write, sizeof(cmd_write));
    if (res == LIBPARAMS_OK) {
        res = spiframTransmit(data, size);
    }
    spiframDeselect();

    return res;
}

static int8_t spiframRead(uint16_t address, uint8_t* data, uint16_t size) {
    if (data == NULL || size == 0U || (uint32_t)address + size > FM25V02_SIZE_BYTES) {
        return LIBPARAMS_WRONG_ARGS;
    }

    const uint8_t cmd_read[3] = {
        SPIFRAM_CMD_READ,
        (uint8_t)(address >> 8U),
        (uint8_t)(address & 0xFFU),
    };

    spiframSelect();
    int8_t res = spiframTransmit(cmd_read, sizeof(cmd_read));
    if (res == LIBPARAMS_OK) {
        res = spiframReceive(data, size);
    }
    spiframDeselect();

    return res;
}

static void flashInit(void) {
    spiframDeselect();
}


static int8_t flashErase(uint32_t start_page_idx, uint32_t num_of_pages) {
    if (num_of_pages == 0U || start_page_idx + num_of_pages > flashGetNumberOfPages()) {
        return LIBPARAMS_WRONG_ARGS;
    }

    uint8_t erased_chunk[SPIFRAM_ERASE_CHUNK_SIZE];
    memset(erased_chunk, 0xFF, sizeof(erased_chunk));

    size_t offset = start_page_idx * flashGetPageSize();
    size_t bytes_left = num_of_pages * flashGetPageSize();

    while (bytes_left > 0U) {
        const size_t chunk_size = (bytes_left < sizeof(erased_chunk)) ?
                                  bytes_left : sizeof(erased_chunk);
        int32_t written = flashWrite(erased_chunk, offset, chunk_size);
        if (written != (int32_t)chunk_size) {
            return (written < 0) ? (int8_t)written : LIBPARAMS_UNKNOWN_HAL_ERROR;
        }

        offset += chunk_size;
        bytes_left -= chunk_size;
    }

    return LIBPARAMS_OK;
}



static int32_t flashWrite(const uint8_t* data, size_t offset, size_t bytes_to_write) {
    if (data == NULL || bytes_to_write == 0U) {
        return LIBPARAMS_WRONG_ARGS;
    }
    const size_t requested_size = bytes_to_write;

    uint16_t address = 0;
    if (spiframNormalizeOffset(offset, &address) != LIBPARAMS_OK) {
        return LIBPARAMS_WRONG_ARGS;
    }
    if ((size_t)address + bytes_to_write > FM25V02_SIZE_BYTES) {
        return LIBPARAMS_WRONG_ARGS;
    }

    while (bytes_to_write > 0U) {
        const size_t chunk_size = (bytes_to_write > UINT16_MAX) ? UINT16_MAX : bytes_to_write;
        int8_t res = spiframWrite(address, data, (uint16_t)chunk_size);
        if (res != LIBPARAMS_OK) {
            return res;
        }

        address = (uint16_t)(address + chunk_size);
        data += chunk_size;
        bytes_to_write -= chunk_size;
    }

    return (int32_t)requested_size;
}

static int8_t flashUnlock(void) {
    return LIBPARAMS_OK;
}

static int8_t flashLock(void) {
    return LIBPARAMS_OK;
}


static size_t flashRead(uint8_t* data, size_t offset, size_t bytes_to_read) {
    if (data == NULL || bytes_to_read == 0U) {
        return 0;
    }

    uint16_t address = 0;
    if (spiframNormalizeOffset(offset, &address) != LIBPARAMS_OK) {
        return 0;
    }
    if ((size_t)address + bytes_to_read > FM25V02_SIZE_BYTES) {
        return 0;
    }

    size_t bytes_left = bytes_to_read;
    while (bytes_left > 0U) {
        const size_t chunk_size = (bytes_left > UINT16_MAX) ? UINT16_MAX : bytes_left;
        int8_t res = spiframRead(address, data, (uint16_t)chunk_size);
        if (res != LIBPARAMS_OK) {
            return 0;
        }

        address = (uint16_t)(address + chunk_size);
        data += chunk_size;
        bytes_left -= chunk_size;
    }

    return bytes_to_read;
}

static uint16_t flashGetNumberOfPages(void) {
    // Two logical regions reuse the parameter store's redundant-page recovery.
    return 2;
}
static uint32_t flashGetPageSize(void) {
    return FM25V02_SIZE_BYTES / 2U;
}

const FlashDriverOps* stm32h753xxSpiFramGetOps(void) {
    static const FlashDriverOps ops = {
        flashInit,
        flashUnlock,
        flashLock,
        flashErase,
        flashWrite,
        flashRead,
        flashGetNumberOfPages,
        flashGetPageSize,
        FLASH_START_ADDR,
    };
    return &ops;
}
