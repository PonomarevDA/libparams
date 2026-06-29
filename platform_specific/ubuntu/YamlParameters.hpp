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

static SimpleLogger logger("libparams-ubuntu");
class YamlParameters {
    ParametersLayout_t params;
    std::string default_file_name = "default_params";  // read-only factory defaults
    std::string nvm_file_name = "nvm_params";           // persistent storage (emulated NVM)

public:
    FlashMemoryLayout_t flash;
    explicit YamlParameters(FlashMemoryLayout_t flash_desc, ParametersLayout_t params_desc);

    int8_t read_from_dir(const std::string& path);
    int8_t write_to_dir(const std::string& path);

    int8_t set_default_file_name(std::string file_name);
    int8_t set_nvm_file_name(std::string file_name);

private:
    int8_t __write_page(std::ofstream& params_storage_file, uint16_t* int_param_idx,
                        uint16_t* str_param_idx);
    int8_t __read_page(std::ifstream& params_storage_file, uint16_t* int_param_idx,
                       uint16_t* str_param_idx);
};

#endif  // LIBPARAM_YAML_PARAMETERS_HPP_
