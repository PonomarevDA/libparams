#include "YamlParameters.hpp"
#include "libparams_error_codes.h"

namespace fs = std::filesystem;

namespace fs = std::filesystem;
extern IntegerDesc_t integer_desc_pool[];
extern StringDesc_t string_desc_pool[];

int8_t YamlParameters::read_from_file(std::string path, uint8_t* flash_memory, size_t pages_n,
                                                    std::tuple<uint8_t, uint8_t>* last_idxs) {
    if (flash_memory == NULL || pages_n < 1 || path.empty()) {
        return LIBPARAMS_WRONG_ARGS;
    }

    std::ifstream params_storage_file;
    params_storage_file.open(path, std::ios_base::in);
    if (!params_storage_file) {
        std::cout << "YamlParameters: " << path << " could not be opened for reading!" << std::endl;
        return LIBPARAMS_WRONG_ARGS;
    }
    std::cout << "YamlParameters: data read from " << path << std::endl;

    size_t int_param_idx = std::get<0>(*last_idxs);
    size_t str_param_idx = std::get<1>(*last_idxs);
    std::string line;

    while (std::getline(params_storage_file, line)) {
        std::istringstream iss(line);
        size_t delimiter_pos = line.find(':');
        std::cout << line << std::endl;
        if (delimiter_pos == std::string::npos) {
            continue;
        }

        std::string value = line.substr(delimiter_pos + 1);
        try {
            std::string num_value;
            std::remove_copy_if(value.begin(), value.end(), num_value.begin(), ::isspace);
            uint32_t int_value = std::stoi(num_value);
            memcpy(flash_memory + 4 * int_param_idx, &int_value, 4);
            int_param_idx++;
        } catch (std::invalid_argument const& ex) {
            size_t quote_pos = value.find('"');
            size_t quote_end_pos = value.find('"', quote_pos + 1);
            std::string str_value = value.substr(quote_pos + 1, quote_end_pos - quote_pos - 1);
            int offset = pages_n * 2048 - MAX_STRING_LENGTH *
                                                        (NUM_OF_STR_PARAMS - str_param_idx);
            memcpy(flash_memory + offset, str_value.c_str(), sizeof(str_value));
            str_param_idx++;
        }
    }
    std::get<0>(*last_idxs) = int_param_idx;
    std::get<1>(*last_idxs) = str_param_idx;
    params_storage_file.close();
    return LIBPARAMS_OK;
}

int8_t YamlParameters::write_to_file(std::string path, uint8_t* flash_memory, size_t pages_n,
                                                std::tuple<uint8_t, uint8_t>* last_idxs) {
    if (flash_memory == NULL || pages_n < 1 || path.empty()) {
        return LIBPARAMS_WRONG_ARGS;
    }

    std::ofstream params_storage_file;
    params_storage_file.open(path, std::ios_base::out);
    if (!params_storage_file) {
        std::cout << "YamlParameters: " << path << " could not be opened for writing!" << std::endl;
        return LIBPARAMS_WRONG_ARGS;
    }
    std::cout << "YamlParameters: save data to " << path << std::endl;
    uint16_t n_bytes = 0;
    for (uint8_t index = std::get<0>(*last_idxs); index < IntParamsIndexes::INTEGER_PARAMS_AMOUNT;
                                                                                        index++) {
        int32_t int_param_value;
        memcpy(&int_param_value, flash_memory + index * 4, 4);
        params_storage_file << std::left << std::setw(32) << integer_desc_pool[index].name << ":\t"
                            << int_param_value << "\n";
        std::cout << std::left << std::setw(32) << integer_desc_pool[index].name
                                                            << ":\t" << int_param_value << "\n";
        n_bytes += 4;
        std::get<0>(*last_idxs) = index + 1;
        if (n_bytes > 2048 - 4) {
            break;
        }
    }

    auto last_str_idx = std::get<1>(*last_idxs);
    auto num_str_params = NUM_OF_STR_PARAMS;
    auto str_params_remained = NUM_OF_STR_PARAMS - last_str_idx;
    auto available_str_params = (2048 - n_bytes) / 56;

    if (available_str_params < str_params_remained) {
        num_str_params = last_str_idx + available_str_params;
    }
    for (uint8_t index = last_str_idx; index < num_str_params; index++) {
        size_t offset = pages_n * 2048 - MAX_STRING_LENGTH * (NUM_OF_STR_PARAMS - index);

        std::string str_param_value(
            reinterpret_cast<char*>(flash_memory + offset), MAX_STRING_LENGTH);
        auto str_end = str_param_value.find('\0');
        auto str_param = str_param_value.substr(0, str_end);
        params_storage_file << std::left << std::setw(32) <<
                            string_desc_pool[index].name <<
                            ":\t" << '"' << str_param.c_str() << '"' << "\n";
        std::cout << std::left << std::setw(32) << string_desc_pool[index].name <<
                            ":\t" << '"' << str_param.c_str() << '"' << "\n";

        n_bytes += MAX_STRING_LENGTH;
        std::get<1>(*last_idxs) += 1;
        if (n_bytes > 2048 - 56) {
            break;
        }
    }

    params_storage_file.close();
    return LIBPARAMS_OK;
}
