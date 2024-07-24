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

#include "storage.h"
#include "libparams_error_codes.h"
#include "YamlParameters.hpp"

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

    char file_name[256];
    uint8_t int_param_idx = 0;
    uint8_t str_param_idx = 0;
    // read params values for each page
    for (uint8_t idx = 0; idx < flash_pages_num; idx++) {
        std::ifstream params_storage_file;

        // check if temp file for the page already exists, else read from init file
        snprintf(file_name, sizeof(file_name), "%s/%s_%d.yml",
                                    path.c_str(), temp_file_name.c_str(), idx);
        params_storage_file.open(file_name, std::ios_base::in);
        if (!params_storage_file) {
            std::cout << "YamlParameters: " << file_name <<
                                                " could not be opened for reading!" << std::endl;
            snprintf(file_name, sizeof(file_name), "%s/%s_%d.yml",
                        path.c_str(), init_file_name.c_str(), idx);
            params_storage_file.open(file_name, std::ios_base::in);
            if (!params_storage_file) {
                std::cout << "YamlParameters: " << file_name <<
                                                " could not be opened for reading!" << std::endl;
                return LIBPARAMS_WRONG_ARGS;
            }
        }

        std::cout << "YamlParameters: data read from " << file_name << std::endl;
        if ((int_param_idx > num_int_params) || (str_param_idx > num_str_params)) {
            break;;
        }
        __read_page(params_storage_file, &int_param_idx, &str_param_idx);
        params_storage_file.close();
    }
    return LIBPARAMS_OK;
}

int8_t YamlParameters::write_to_files(const std::string& path) {
    if (path.empty()) {
        return LIBPARAMS_WRONG_ARGS;
    }

    char file_name[256];
    // remember last written indexes
    uint8_t int_param_idx = 0;
    uint8_t str_param_idx = 0;
    for (uint8_t idx = 0; idx < flash_pages_num; idx++) {
        snprintf(file_name, sizeof(file_name), "%s/%s_%d.yml",
                                    path.c_str(), temp_file_name.c_str(), idx);
        std::ofstream params_storage_file;
        params_storage_file.open(file_name, std::ios_base::out);
        std::cout << "YamlParameters: save data to " << file_name << std::endl;

        __write_page(params_storage_file, &int_param_idx, &str_param_idx);
        params_storage_file.close();
    }
    if (int_param_idx < num_int_params || str_param_idx < num_str_params) {
        return LIBPARAMS_WRONG_ARGS;
    }
    return LIBPARAMS_OK;
}

void YamlParameters::__read_page(std::ifstream& params_storage_file, uint8_t* int_param_idx,
                                                                    uint8_t* str_param_idx){
    std::string line;
    while (std::getline(params_storage_file, line)) {
        std::istringstream iss(line);
        size_t delimiter_pos = line.find(':');
        // std::cout << line << std::endl;
        if (delimiter_pos == std::string::npos) {
            continue;
        }
        std::string value = line.substr(delimiter_pos + 1);
        try {
            std::string num_value;
            std::remove_copy_if(value.begin(), value.end(), num_value.begin(), ::isspace);
            int32_t int_value = std::stoi(num_value);
            if (flash_size < 4 * (*int_param_idx)) {
                throw std::invalid_argument(
                                std::string("YamlParameters: Not enought flash size"));
            }
            memcpy(flash_memory + 4 * (*int_param_idx), &int_value, 4);
            *int_param_idx = *int_param_idx + 1;
            if (*int_param_idx > num_int_params) {
                throw std::invalid_argument(
                                std::string("YamlParameters: Wrong num_int_params"));
            }
        } catch (std::invalid_argument const& ex) {
            if (flash_size < uint(*int_param_idx * 4 + MAX_STRING_LENGTH *(*str_param_idx))) {
                char error_mesg[100];
                snprintf(error_mesg, sizeof(error_mesg),
                    "YamlParameters: Not enought flash size, needed: %d, provided: %d",
                    *int_param_idx * 4 + MAX_STRING_LENGTH *(*str_param_idx), (int)flash_size);
                throw std::invalid_argument(std::string(error_mesg));
            }
            size_t quote_pos = value.find('"');
            size_t quote_end_pos = value.find('"', quote_pos + 1);
            std::string str_value = value.substr(quote_pos + 1, quote_end_pos - quote_pos - 1);
            int offset = flash_pages_num * page_size - MAX_STRING_LENGTH *
                                                        (num_str_params - (*str_param_idx));
            if (offset < *int_param_idx * 4) {
                char error_mesg[100];
                snprintf(error_mesg, sizeof(error_mesg),
                    "YamlParameters: Not enought flash size, needed: %d, provided: %d",
                    *int_param_idx * 4 + MAX_STRING_LENGTH *(*str_param_idx), (int)flash_size);
                throw std::invalid_argument(std::string(error_mesg));
            }
            if (*str_param_idx > num_str_params) {
                throw std::invalid_argument(
                                std::string("YamlParameters: Wrong num_str_params"));
            }
            memcpy(flash_memory + offset, str_value.c_str(), strlen(str_value.c_str()));
            memcpy(flash_memory + offset + strlen(str_value.c_str()), "\0", 1);
            *str_param_idx = *str_param_idx + 1;
        }
    }
}

void YamlParameters::__write_page(std::ofstream& params_storage_file, uint8_t* int_param_idx,
                                                                    uint8_t* str_param_idx) {
    uint32_t n_bytes = 0;
    uint8_t param_idx = *int_param_idx;
    for (uint8_t index = param_idx; index < num_int_params; index++) {
        int32_t int_param_value;
        memcpy(&int_param_value, flash_memory + index * 4, 4);
        const IntegerDesc_t* param_desc = paramsGetIntegerDesc(index);
        if (param_desc == NULL) {
            throw std::invalid_argument(
                            std::string("YamlParameters: Wrong param idx"));
        }
        const char* name = paramsGetIntegerDesc(index)->name;
        params_storage_file << std::left << std::setw(32) << name << ":\t"
                            << int_param_value << "\n";
        std::cout << std::left << std::setw(32) << name << ":\t" << int_param_value << "\n";
        n_bytes += 4;
        *int_param_idx = *int_param_idx + 1;
        if (n_bytes + 4 > page_size) {
            break;
        }
    }

    auto prev_str_idx = *str_param_idx;
    auto last_str_param_idx = num_str_params;
    auto str_params_remained = num_str_params - prev_str_idx;
    int available_str_params = (page_size - n_bytes) / 56;

    if (available_str_params < str_params_remained) {
        last_str_param_idx = prev_str_idx + available_str_params;
    }
    for (uint8_t index = prev_str_idx; index < last_str_param_idx; index++) {
        size_t offset = flash_pages_num * page_size - MAX_STRING_LENGTH * (num_str_params - index);

        std::string str_param_value(
            reinterpret_cast<char*>(flash_memory + offset), MAX_STRING_LENGTH);
        auto str_end = str_param_value.find('\0');
        auto str_param = str_param_value.substr(0, str_end);
        const char* name = paramsGetStringDesc(index + num_int_params)->name;

        params_storage_file << std::left << std::setw(32) << name << ":\t" << '"'
                                                            << str_param.c_str() << '"' << "\n";
        std::cout << std::left << std::setw(32) << name << ":\t" << '"'
                                                            << str_param.c_str() << '"' << "\n";

        n_bytes += MAX_STRING_LENGTH;

        *str_param_idx = *str_param_idx + 1;
        if (n_bytes + 56 > page_size) {
            break;
        }
    }
}
