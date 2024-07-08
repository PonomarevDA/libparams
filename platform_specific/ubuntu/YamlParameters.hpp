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
#include <iostream>
#include <cstdio>            // C system headers
#include <cstdlib>           // C++ system headers
#include <string>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <filesystem>
#include "storage.h"
#include "libparams_error_codes.h"  // Other libraries' headers
#include "params.hpp"

#ifdef __cplusplus
extern "C" {
#endif

class YamlParameters {
public:
    static void read_from_file(uint8_t* flash_memory, std::ifstream & params_storage_file);
    static std::tuple<uint8_t, uint8_t> write_to_file(uint8_t* flash_memory,
        std::ofstream& params_storage_file, std::tuple<uint8_t, uint8_t>  last_idxs);
};


#ifdef __cplusplus
}
#endif

#endif  // LIBPARAM_YAML_PARAMETERS_HPP_
