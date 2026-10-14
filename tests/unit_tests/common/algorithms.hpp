/*
 * Copyright (c) 2022-2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef LIBPARAM_UNIT_TESTS_ALGHORITMS_HPP_
#define LIBPARAM_UNIT_TESTS_ALGHORITMS_HPP_

#ifdef __cplusplus
extern "C" {
#endif

// size is the buffer size: size - 1 random characters and the terminating zero
inline void generateRandomCString(char* str, int size) {
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    int charsetSize = sizeof(charset) - 1;

    for (int i = 0; i < size - 1; ++i) {
        str[i] = charset[rand() % charsetSize];
    }
    str[size - 1] = '\0';
}

#ifdef __cplusplus
}
#endif

#endif  // LIBPARAM_UNIT_TESTS_ALGHORITMS_HPP_
