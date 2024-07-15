/*
 * Copyright (c) 2024 Anastasiia Stepanova <asiiapine@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <gtest/gtest.h>
#include <iostream>
#include "YamlParameters.hpp"
#include "libparams_error_codes.h"

// Test Case 1. Initialize flash driver
TEST(TestFlashDriver, initializeFlashDriver) {
    flashUnlock();
    auto res = flashErase(0, 1);
    flashLock();
    ASSERT_EQ(res, LIBPARAMS_OK);
}

int main (int argc, char *argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
