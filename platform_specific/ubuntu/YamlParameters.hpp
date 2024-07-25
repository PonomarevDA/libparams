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

class YamlParameters {
    uint8_t* flash_memory;
    uint16_t page_size;
    uint8_t flash_pages_num;
    uint8_t num_str_params;
    uint8_t num_int_params;
    uint32_t flash_size;
    std::string init_file_name = "init_params";
    std::string temp_file_name = "temp_params";

   public:
    YamlParameters(uint8_t* flash_memory_ptr, uint16_t page_size_bytes, uint8_t num_flash_pages,
                                                uint8_t str_params_num, uint8_t int_params_num)
    : flash_memory(flash_memory_ptr), page_size(page_size_bytes), flash_pages_num(num_flash_pages),
    num_str_params(str_params_num), num_int_params(int_params_num) {
        if (flash_memory_ptr == nullptr || page_size_bytes < 4 || num_flash_pages == 0) {
            throw std::invalid_argument("YamlParameters: Invalid arguments for constructor");
        }
        flash_size = page_size * flash_pages_num;
        uint32_t req_flash_size = num_flash_pages * 4 + 56 * num_str_params;
        if (flash_size < req_flash_size) {
                char error_mesg[100];
                snprintf(error_mesg, sizeof(error_mesg),
                    "YamlParameters: Not enought flash size, needed: %d, provided: %d\n",
                    (int)req_flash_size, (int)flash_size);
                std::cout << error_mesg;
                throw std::invalid_argument(error_mesg);
        }
    }

    int8_t read_from_dir(const std::string& path);
    int8_t write_to_dir(const std::string& path);

    int8_t set_init_file_name(std::string file_name);
    int8_t set_temp_file_name(std::string file_name);

private:
    int8_t __write_page(std::ofstream& params_storage_file, uint8_t* int_param_idx,
                                                                    uint8_t* str_param_idx);
    int8_t __read_page(std::ifstream& params_storage_file, uint8_t* int_param_idx,
                                                                    uint8_t* str_param_idx);
};

#endif  // LIBPARAM_YAML_PARAMETERS_HPP_
