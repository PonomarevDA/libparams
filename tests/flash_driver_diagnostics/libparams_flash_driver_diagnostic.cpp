/// This software is distributed under the terms of the MIT License.
/// Copyright (c) 2022 Dmitry Ponomarev.
/// Author: Dmitry Ponomarev <ponomarevda96@gmail.com>

#include "libparams_flash_driver_diagnostic.hpp"
#include <array>
#include "main.h"
#include "flash_driver.h"
#include "libparams_error_codes.h"


/**
 * @arg[in] Virtual (continuous) page index from 0 to 256
 * @arg[out] Actual (real) page address from 0x08000000 to 0x0807F800
 */
uint32_t mapVirtualPageIndexToAddress(uint16_t virtual_page_index) {
    return 0x08000000 + 2048 * virtual_page_index;
}

/**
 * @brief Write to the specified address, read the value of the specified value and compare them
 * @return 0 on success and -1 on failure
 */
int8_t libparamsWriteAndRead(uint32_t addr, uint64_t value) {
    flashUnlock();
#if defined(STM32F103xB)
    auto res = flashWriteU32(addr, value);
#elif defined(STM32G0B1xx)
    auto res = flashWriteU64(addr, value);
#endif
    flashLock();
    if (res < 0) {
        return res;
    }

    if ((*((uint64_t*)addr)) != value) {
        res = -1;
    }

    return res;
}

/**
 * @brief 1.
 */
int8_t libparamsTestAddressConversion() {
    std::array<std::pair<uint16_t, size_t>, 5> page_addresses_table = { {
        {0,     0x08000000},
        {1,     0x08000800},
        {127,   0x0803F800},
        {128,   0x08040000},
        {255,   0x0807F800},
    } };

    for (auto page_address : page_addresses_table) {
        if (mapVirtualPageIndexToAddress(page_address.first) != page_address.second) {
            return -1;
        }
    }

    return 0;
}

/**
 * @brief 2.
 */
int8_t libparamsTestWriteReadOnLastPageOfFirstBank() {
    int8_t res = 0;
    uint32_t first_var_addr = mapVirtualPageIndexToAddress(127);
    uint32_t second_var_addr = mapVirtualPageIndexToAddress(127) + 8;

    flashUnlock();
    res = flashErase(127, 1);
    flashLock();
    if (res < 0) {
        return res;
    }

    res = libparamsWriteAndRead(first_var_addr, 1234567);
    if (res < 0) {
        return res;
    }

    res = libparamsWriteAndRead(second_var_addr, 2345678);
    if (res < 0) {
        return res;
    }

    return res;
}

/**
 * @brief 3.
 */
int8_t libparamsTestWriteReadOnLastPageOfSecondBank() {
    int8_t res;
    uint32_t first_var_addr = mapVirtualPageIndexToAddress(255);
    uint32_t second_var_addr = mapVirtualPageIndexToAddress(255) + 8;

    flashUnlock();
    res = flashErase(255, 1);
    flashLock();
    if (res < 0) {
        return res;
    }

    res = libparamsWriteAndRead(first_var_addr, 1234567);
    if (res < 0) {
        return res;
    }

    res = libparamsWriteAndRead(second_var_addr, 2345678);
    if (res < 0) {
        return res;
    }

    return res;
}

/**
 * @brief 4.
 */
int8_t libparamsTestWriteReadOnMultiplePagesErase() {
    int8_t res;
    uint32_t first_var_addr = mapVirtualPageIndexToAddress(126);
    uint32_t second_var_addr = mapVirtualPageIndexToAddress(127);

    flashUnlock();
    res = flashErase(126, 2);
    flashLock();
    if (res < 0) {
        return res;
    }

    res = libparamsWriteAndRead(first_var_addr, 1234567);
    if (res < 0) {
        return res;
    }

    res = libparamsWriteAndRead(second_var_addr, 2345678);
    if (res < 0) {
        return res;
    }

    return res;
}

/**
 * @brief 5.
 */
int8_t libparamsTestWriteReadOnBothBanks() {
    int8_t res;
    uint32_t first_var_addr = mapVirtualPageIndexToAddress(127);
    uint32_t second_var_addr = mapVirtualPageIndexToAddress(128);
    uint32_t third_var_addr = mapVirtualPageIndexToAddress(129);

    flashUnlock();
    res = flashErase(127, 3);
    flashLock();
    if (res < 0) {
        return res;
    }

    res = libparamsWriteAndRead(first_var_addr, 1234567);
    if (res < 0) {
        return res;
    }

    res = libparamsWriteAndRead(second_var_addr, 2345678);
    if (res < 0) {
        return res;
    }

    res = libparamsWriteAndRead(third_var_addr, 42);
    if (res < 0) {
        return res;
    }

    return res;
}

typedef int8_t (*FlashDriverSelfTest)();
std::array<FlashDriverSelfTest, 5> tests {
    {
        &libparamsTestAddressConversion,
        &libparamsTestWriteReadOnLastPageOfFirstBank,
        &libparamsTestWriteReadOnLastPageOfSecondBank,
        &libparamsTestWriteReadOnMultiplePagesErase,
        &libparamsTestWriteReadOnBothBanks
    }
};


DiagnosticResult libparamsRunFlashDriverDiagnostic() {
    for (auto test : tests) {
        if (test() < 0) {
            return DiagnosticResult::FAILURE;
        }
    }

    return DiagnosticResult::OK;
}
