/*
 * Copyright (c) 2024 Anastasiia Stepanova <asiiapine@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */


#ifndef LIBPARAM_YAML_PARAMETERS_HPP_
#define LIBPARAM_YAML_PARAMETERS_HPP_

#include <iostream>
#include <cstdio>            // C system headers
#include <cstdlib>           // C++ system headers
#include <string>
#include <fstream>
#include <iomanip>
#include <filesystem>
#include "libparams_error_codes.h"  // Other libraries' headers
#include "storage.h"
#include "params.hpp"

#ifdef __cplusplus
extern "C" {
#endif

class YamlParameters {
// public:
    static void read_from_file(RomDriverInstance rom, char* file_dir);
    static void write_to_file(RomDriverInstance rom, char* file_dir);
};


#ifdef __cplusplus
}
#endif

#endif  // LIBPARAM_YAML_PARAMETERS_HPP_
