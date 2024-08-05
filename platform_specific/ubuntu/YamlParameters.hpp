/*
 * Copyright (c) 2024 Anastasiia Stepanova <asiiapine@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef LIBPARAM_YAML_PARAMETERS_HPP_
#define LIBPARAM_YAML_PARAMETERS_HPP_

#include <string.h>
#include "SimpleLogger.hpp"
#include "FlashMemoryLayout.hpp"

static SimpleLogger logger("YamlParameters");
class YamlParameters {
    FlashMemoryLayout_t flash;
    ParametersLayout_t params;
    std::string init_file_name = "init_params";
    std::string temp_file_name = "temp_params";

public:
    explicit YamlParameters(FlashMemoryLayout_t flash_desc, ParametersLayout_t params_desc);

    int8_t read_from_dir(const std::string& path);
    int8_t write_to_dir(const std::string& path);

    int8_t set_init_file_name(std::string file_name);
    int8_t set_temp_file_name(std::string file_name);

private:
    int8_t __write_page(std::ofstream& params_storage_file, uint16_t* int_param_idx,
                        uint16_t* str_param_idx);
    int8_t __read_page(std::ifstream& params_storage_file, uint16_t* int_param_idx,
                       uint16_t* str_param_idx);
};

#endif  // LIBPARAM_YAML_PARAMETERS_HPP_
