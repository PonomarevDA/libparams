/// This software is distributed under the terms of the MIT License.
/// Copyright (c) 2022 Dmitry Ponomarev.
/// Author: Dmitry Ponomarev <ponomarevda96@gmail.com>

#ifndef LINPARAMS_FLASH_DRIVER_DIAGNOSTIC_HPP_
#define LINPARAMS_FLASH_DRIVER_DIAGNOSTIC_HPP_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

enum class DiagnosticResult {
    OK,
    FAILURE,
};

/**
 * @brief Perform self-test diagnostic of flash driver. The test sequence are following:
 * 1. Internal tests
 * 2. Test WriteAndRead on 127 page (last page of the first bank)
 * 3. Test WriteAndRead on 255 page (last page of the second bank)
 * 4. Test WriteAndRead on 126-127 pages (multiple pages of the first bank)
 * 5. Test WriteAndRead on 126-128 pages (multiple pages of the both banks)
 * @note The function corrupts the uploaded firmware.
 */
DiagnosticResult libparamsRunFlashDriverDiagnostic();

void led_toggle_blue();

#ifdef __cplusplus
}
#endif

#endif  // LINPARAMS_FLASH_DRIVER_DIAGNOSTIC_HPP_
