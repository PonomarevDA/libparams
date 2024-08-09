/*
 * Copyright (c) 2024 Anastasiia Stepanova <asiiapine@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iomanip>

#include "storage.h"
#include "YamlParameters.hpp"

YamlParameters::YamlParameters(FlashMemoryLayout_t flash_desc,
                               ParametersLayout_t params_desc) {
    char error_mesg[100];
    flash = flash_desc;
    params = params_desc;
    if (flash.memory_ptr == nullptr || flash.page_size < 4 || flash.num_pages == 0) {
        throw std::invalid_argument("Invalid arguments for constructor");
    }
    uint32_t req_flash_size = params.num_int_params * 4 + 56 * params.num_str_params;
    if (flash.flash_size < req_flash_size) {
        snprintf(error_mesg, sizeof(error_mesg),
                 "Not enought flash size, needed: %d, provided: %d",
                 (int)req_flash_size, (int)flash.flash_size);
        throw std::invalid_argument(error_mesg);
    }
    const char* last_int_name = params.integer_desc_pool[params.num_int_params - 1].name;
    const char* last_str_name = params.string_desc_pool[params.num_str_params - 1].name;
    if ((last_int_name == nullptr && params.num_int_params > 0) ||
            (last_str_name == nullptr && params.num_str_params > 0)) {
        throw std::invalid_argument("Wrong number of parameters");
    }
}

int8_t YamlParameters::set_init_file_name(std::string file_name) {
    if (file_name.empty()) {
        return LIBPARAMS_WRONG_ARGS;
    }
    init_file_name = file_name;
    return LIBPARAMS_OK;
}

int8_t YamlParameters::set_temp_file_name(std::string file_name) {
    if (file_name.empty()) {
        return LIBPARAMS_WRONG_ARGS;
    }
    temp_file_name = file_name;
    return LIBPARAMS_OK;
}

int8_t YamlParameters::read_from_dir(const std::string& path) {
    if (path.empty()) {
        return LIBPARAMS_WRONG_ARGS;
    }
    int8_t res;
    char file_name[256];
    uint16_t int_param_idx = 0;
    uint16_t str_param_idx = 0;
    // read params values for each page
    for (uint16_t idx = 0; idx < flash.num_pages; idx++) {
        std::ifstream params_storage_file;

        // check if temp file for the page already exists, else read from init file
        snprintf(file_name, sizeof(file_name), "%s/%s_%d.yaml",
                 path.c_str(), temp_file_name.c_str(), idx);
        params_storage_file.open(file_name, std::ios_base::in);
        if (!params_storage_file) {
            logger.info(file_name, " could not be opened for reading!");
            snprintf(file_name, sizeof(file_name), "%s/%s_%d.yaml",
                     path.c_str(), init_file_name.c_str(), idx);
            params_storage_file.open(file_name, std::ios_base::in);
            if (!params_storage_file) {
                logger.error(file_name, " could not be opened for reading!");
                return LIBPARAMS_WRONG_ARGS;
            }
        }
        logger.info("data read from ", file_name);

        if ((int_param_idx > params.num_int_params) || (str_param_idx > params.num_str_params)) {
            break;
        }
        res = __read_page(params_storage_file, &int_param_idx, &str_param_idx);
        params_storage_file.close();
        if (res != LIBPARAMS_OK) {
            return res;
        }
    }

    if (int_param_idx != params.num_int_params || str_param_idx != params.num_str_params) {
        logger.error("Number of parameters in the file isn't equal to",
                     " the one specified in the constructor\n",
                     "int real: ", (int)int_param_idx, " expected: ", (int)params.num_int_params,
                     "\n",
                     "str real: ", (int)str_param_idx, " expected: ", (int)params.num_str_params);
        return LIBPARAMS_WRONG_ARGS;
    }
    return LIBPARAMS_OK;
}

int8_t YamlParameters::write_to_dir(const std::string& path) {
    if (path.empty()) {
        return LIBPARAMS_WRONG_ARGS;
    }
    int8_t res;
    char file_name[256];
    // remember last written indexes
    uint16_t int_param_idx = 0;
    uint16_t str_param_idx = 0;
    for (uint16_t idx = 0; idx < flash.num_pages; idx++) {
        snprintf(file_name, sizeof(file_name), "%s/%s_%d.yaml",
                 path.c_str(), temp_file_name.c_str(), idx);
        std::ofstream params_storage_file;
        params_storage_file.open(file_name, std::ios_base::out);
        logger.info("save data to ", file_name);
        res = __write_page(params_storage_file, &int_param_idx, &str_param_idx);
        params_storage_file.close();
        if (res != LIBPARAMS_OK) {
            return res;
        }
    }
    if (int_param_idx != params.num_int_params || str_param_idx != params.num_str_params) {
        logger.error("Number of parameters in the file isn't equal",
                     " to the one specified in the constructor",
                     "int real: ", (int)int_param_idx, " expected: ", (int)params.num_int_params,
                     "\n",
                     "str real: ", (int)str_param_idx, " expected: ", (int)params.num_str_params);
        return LIBPARAMS_WRONG_ARGS;
    }
    return LIBPARAMS_OK;
}

int8_t YamlParameters::__read_page(std::ifstream& params_storage_file, uint16_t* int_param_idx,
                                   uint16_t* str_param_idx) {
    std::string line;
    std::string value;

    while (std::getline(params_storage_file, line)) {
        logger.info(line);
        size_t delimiter_pos = line.find(':');
        if (delimiter_pos == std::string::npos) {
            continue;
        }
        value = line.substr(delimiter_pos + 1);
        try {
            if ((*int_param_idx) > params.num_int_params) {
                logger.error("Got more integer params than defined by num_int_params");
                return LIBPARAMS_WRONG_ARGS;
            }
            if (flash.flash_size < 4 * (*int_param_idx)) {
                logger.error("Not enought flash size");
                return LIBPARAMS_WRONG_ARGS;
            }
            int32_t int_value = std::stoi(value);
            memcpy((void*)(flash.memory_ptr + 4 * (*int_param_idx)), &int_value, 4);
            *int_param_idx = *int_param_idx + 1;
        } catch (std::invalid_argument const& ex) {
            uint32_t offset = flash.flash_size - MAX_STRING_LENGTH *
                              (params.num_str_params - (*str_param_idx ));
            if ((*str_param_idx) >= params.num_str_params) {
                logger.error("Wrong num_str_params expected: ", params.num_str_params,
                             " got: ", (*str_param_idx));
                return LIBPARAMS_WRONG_ARGS;
            }

            size_t quote_pos = value.find('"');
            size_t quote_end_pos = value.find('"', quote_pos + 1);
            std::string str_value = value.substr(quote_pos + 1, quote_end_pos - quote_pos - 1);
            if (offset < (*int_param_idx) * 4) {
                logger.error("params overlap last int param addr", *int_param_idx * 4,
                             ", str param offset ", offset);
                return LIBPARAMS_WRONG_ARGS;
            }

            memcpy((void*)(flash.memory_ptr + offset), str_value.c_str(),
                   strlen(str_value.c_str()));
            memcpy((void*)(flash.memory_ptr + offset + strlen(str_value.c_str())), "\0", 1);
            *str_param_idx = *str_param_idx + 1;
        }
    }

    return LIBPARAMS_OK;
}

int8_t YamlParameters::__write_page(std::ofstream& params_storage_file, uint16_t* int_param_idx,
                                    uint16_t* str_param_idx) {
    if (*int_param_idx > params.num_int_params || *str_param_idx > params.num_str_params) {
        logger.error("int_param_idx or str_param_idx is bigger than defined by num_int_params\n");
        return LIBPARAMS_WRONG_ARGS;
    }

    uint32_t n_bytes = 0;
    uint16_t param_idx = *int_param_idx;

    for (uint16_t index = param_idx; index < params.num_int_params; index++) {
        int32_t int_param_value;
        const char* name = params.integer_desc_pool[index].name;
        memcpy(&int_param_value, flash.memory_ptr + index * 4, 4);
        params_storage_file << std::left << std::setw(32) << name << ": "
                            << int_param_value << "\n";
        logger.info(std::left, std::setw(32), name, ":\t", int_param_value);
        n_bytes += 4;
        *int_param_idx = *int_param_idx + 1;
        if (n_bytes + 4 > flash.page_size) {
            return LIBPARAMS_OK;
        }
    }

    auto prev_str_idx = *str_param_idx;
    auto last_str_param_idx = params.num_str_params;
    auto str_params_remained = params.num_str_params - prev_str_idx;
    int available_str_params = (flash.page_size - n_bytes) / 56;

    if (available_str_params < str_params_remained) {
        last_str_param_idx = prev_str_idx + available_str_params;
    }
    for (uint16_t index = prev_str_idx; index < last_str_param_idx; index++) {
        const char* name = params.string_desc_pool[index].name;
        if (name == nullptr) {
            return LIBPARAMS_OK;
        }
        size_t offset = flash.flash_size - MAX_STRING_LENGTH * (params.num_str_params - index);

        std::string str_param_value(reinterpret_cast<char*>((void*)(flash.memory_ptr + offset)),
                                    MAX_STRING_LENGTH);
        auto str_end = str_param_value.find('\0');
        auto str_param = str_param_value.substr(0, str_end);
        params_storage_file << std::left << std::setw(32) << name << ": " << '"'
                            << str_param.c_str() << '"' << "\n";
        logger.info(std::left, std::setw(32), name, ":\t", '"', str_param.c_str(), '"');

        n_bytes += MAX_STRING_LENGTH;

        *str_param_idx = *str_param_idx + 1;
        if (n_bytes + 56 > flash.page_size) {
            return LIBPARAMS_OK;
        }
    }
    return LIBPARAMS_OK;
}
