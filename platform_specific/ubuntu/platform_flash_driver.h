/*
 * Copyright (c) 2026 Ilia Kliantsevich <iliawork112005@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef LIBPARAMS_UBUNTU_PLATFORM_FLASH_DRIVER_H_
#define LIBPARAMS_UBUNTU_PLATFORM_FLASH_DRIVER_H_

#include "flash_driver.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    LIBPARAMS_LOG_NONE = 0,
    LIBPARAMS_LOG_ERROR = 1,
    LIBPARAMS_LOG_INFO = 2,     ///< Params dir, files read, changed values.
    LIBPARAMS_LOG_DEBUG = 3,    ///< Full parameter table, every flash erase/write.
} LibparamsLogLevel;

/**
 * @brief Build-time default log level, e.g. -DLIBPARAMS_LOG_LEVEL=1 to keep only errors.
 */
#ifndef LIBPARAMS_LOG_LEVEL
#define LIBPARAMS_LOG_LEVEL LIBPARAMS_LOG_INFO
#endif

typedef void (*LibparamsLogHandler)(LibparamsLogLevel level, const char* message);

const FlashDriverOps* ubuntuFlashGetOps(void);

/**
 * @brief Drop messages above the level. Call it before paramsInit() to quiet the initial load.
 */
void ubuntuFlashSetLogLevel(LibparamsLogLevel level);

/**
 * @brief Route messages to the handler instead of stdout/stderr. NULL restores the default.
 * The level filter is applied before the handler is called.
 */
void ubuntuFlashSetLogHandler(LibparamsLogHandler handler);

#ifdef __cplusplus
}
#endif

#endif  // LIBPARAMS_UBUNTU_PLATFORM_FLASH_DRIVER_H_
