#include "YamlParameters.hpp"

namespace fs = std::filesystem;

namespace fs = std::filesystem;
extern IntegerDesc_t integer_desc_pool[];
extern StringDesc_t string_desc_pool[];

void YamlParameters::read_from_file(uint8_t* flash_memory,
                                    std::ifstream& params_storage_file) {
    size_t int_param_idx = 0;
    size_t str_param_idx = 0;
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
            value.erase(std::remove_if(value.begin(), value.end(), ::isspace),
                        value.end());
            uint32_t int_param_value = std::stoi(value);
            memcpy(flash_memory + 4 * int_param_idx, &int_param_value, 4);
            int_param_idx++;
        } catch (std::invalid_argument const& ex) {
            size_t offset = 2048 - MAX_STRING_LENGTH * (1 + str_param_idx);
            memcpy(flash_memory + offset, value.c_str(), sizeof(value));
            str_param_idx++;
        }
    }
}

void YamlParameters::write_to_file(uint8_t* flash_memory,
                                   std::ofstream& params_storage_file) {
    for (size_t index = 0; index < IntParamsIndexes::INTEGER_PARAMS_AMOUNT;
         index++) {
        int32_t int_param_value;
        memcpy(&int_param_value, flash_memory + index * 4, 4);
        params_storage_file << std::left << std::setw(32) << integer_desc_pool[index].name << ":\t"
                                                                        << int_param_value << "\n";
        std::cout << std::left << std::setw(32) << integer_desc_pool[index].name
                                                                << ":\t" << int_param_value << "\n";
    }

    for (size_t index = 0; index < NUM_OF_STR_PARAMS; index++) {
        std::string str_param_value(
            reinterpret_cast<char*>(flash_memory + 2048 -  (NUM_OF_STR_PARAMS - index) *
                                                MAX_STRING_LENGTH), MAX_STRING_LENGTH);
        auto str_end = str_param_value.find('\0');
        str_param_value = str_param_value.substr(0, str_end);
        params_storage_file << std::left << std::setw(32) << string_desc_pool[index].name << ":\t"
                                                                        << str_param_value << "\n";
        std::cout << std::left << std::setw(32) << string_desc_pool[index].name
                                                                << ":\t" << str_param_value << "\n";
    }
    params_storage_file.write("\n", 1);
}
