/*
 * Copyright (c) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef LIBPARAMS_ERROR_CODES_H_
#define LIBPARAMS_ERROR_CODES_H_

// HAL related error codes [-1; -19]
#define LIBPARAMS_OK                    0
#define LIBPARAMS_WRONG_ARGS            -2
#define LIBPARAMS_UNKNOWN_HAL_ERROR     -4
#define LIBPARAMS_NOT_INITIALIZED       -8

// Flash memory related error codes [-20; -39]

// ROM related error codes [-40; -59]
#define LIBPARAMS_ROM_WRITE_BAD_ARGS_ERROR  -40
#define LIBPARAMS_ROM_WRITE_PROTECTED_ERROR -41
#define LIBPARAMS_ROM_WRITE_UNKNOWN_ERROR   -59


// Storage related errors [-60; -79]

#define LIBPARAMS_UNKNOWN_ERROR         -128

#endif  // LIBPARAMS_ERROR_CODES_H_
