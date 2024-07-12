/*
 * Copyright (c) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <iostream>
#include "flash_driver.h"
#include "libparams_error_codes.h"
#include "params.hpp"

extern StringParamValue_t string_values_pool[];
extern uint8_t flash_memory[2048];


int main (int argc, char *argv[]) {
    paramsInit(IntParamsIndexes::INTEGER_PARAMS_AMOUNT, NUM_OF_STR_PARAMS, -1, 1);
    paramsLoad();

    std::cout << "Integer parameters:" << std::endl;
    for (size_t int_idx = 0; int_idx < IntParamsIndexes::INTEGER_PARAMS_AMOUNT; int_idx++) {
        auto integer_desc = paramsGetIntegerDesc(int_idx);
        std::cout << "- " << integer_desc->name << ": " << paramsGetIntegerValue(int_idx) <<
                                                                                    std::endl;
    }

    std::cout << "String parameters:" << std::endl;
    for (size_t str_idx = IntParamsIndexes::INTEGER_PARAMS_AMOUNT; str_idx <
                IntParamsIndexes::INTEGER_PARAMS_AMOUNT + NUM_OF_STR_PARAMS; str_idx++) {
        std::cout << "- ";
        auto string_desc = paramsGetStringDesc(str_idx);
        auto string_param_ptr = paramsGetStringValue(str_idx);
        if (string_param_ptr != nullptr) {
            std::cout << string_desc->name << ": " << *string_param_ptr << std::endl;
        } else {
            std::cout << "nullptr" << std::endl;
        }
    }
}
