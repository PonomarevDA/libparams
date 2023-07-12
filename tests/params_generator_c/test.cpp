/*
 * Copyright (C) 2020-2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
 * Copyright (C) 2020-2023 Raccoon Lab team
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <iostream>
#include <vector>
#include <gtest/gtest.h>
#include "storage.h"
#include "params.h"
#include "string_params.h"

bool strCompareSafe(const char* first, const char* second, size_t len) {
    for (size_t idx = 0; idx < len; idx++) {
        if (first[idx] != second[idx]) {
            return false;
        }
    }

    return true;
}

void init() {
    romInit(0, 1);
    paramsInit(IntParamsIndexes::INTEGER_PARAMS_AMOUNT, NUM_OF_STR_PARAMS);
    paramsLoadFromFlash();
}

TEST(TestParamsGenerator, test_str_params_amount) {
    ASSERT_EQ(NUM_OF_STR_PARAMS, 3);
}

TEST(TestParamsGenerator, test_int_params_amount) {
    ASSERT_EQ(static_cast<uint8_t>(IntParamsIndexes::INTEGER_PARAMS_AMOUNT), 3);
}

TEST(TestParamsGenerator, test_int_param) {
    init();
    ParamIndex_t idx = IntParamsIndexes::BAROMETER_PRESSURE_ID;
    IntegerParamValue_t value = paramsGetIntegerValue(idx);
    ASSERT_EQ(value, 0);
}

TEST(TestParamsGenerator, test_str_get_out_of_bounds) {
    init();
    ASSERT_EQ(NULL, (char*)paramsGetStringValue(0));
    ASSERT_EQ(NULL, (char*)paramsGetStringValue(1));
    ASSERT_EQ(NULL, (char*)paramsGetStringValue(2));
    ASSERT_EQ(NULL, (char*)paramsGetStringValue(6));
}

TEST(TestParamsGenerator, test_str_get_persistent_params) {
    init();

    std::vector<std::pair<int, const char*> > reference = {
        std::make_pair(3, "uavcan.si.sample.pressure.Scalar"),
        std::make_pair(4, "uavcan.si.sample.temperature.Scalar"),
        std::make_pair(5, "uavcan.si.sample.magnetic_field_strength.Vector3")
    };

    for (auto ref : reference) {
        auto ref_len = strlen(ref.second);
        auto actual_len = strlen((char*)paramsGetStringValue(ref.first));
        ASSERT_EQ(ref_len, actual_len);
    }
}

int main (int argc, char *argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
