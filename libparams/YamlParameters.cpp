#include "YamlParamerers.hpp"

namespace fs = std::filesystem;

extern uint8_t flash_memory;
namespace fs = std::filesystem;
extern IntegerDesc_t integer_desc_pool[];
extern StringDesc_t string_desc_pool[];

void YamlParameters::read_from_file(RomDriverInstance rom, char* file_dir){
    std::ifstream params_storage_file;
    fs::directory_entry entry{file_dir};
    if (!entry.exists()) {
        return;
    }
    params_storage_file.open(file_dir, std::ios::in);
    if (!params_storage_file) {
        std::cout << "Flash driver: " << file_dir << " been found!"
                  << std::endl;
    }
    size_t read_counter = 0;
    size_t int_param_idx = 0;
    size_t str_param_idx = 0;
    while (params_storage_file) {
        if (read_counter % 2 == 0) {
            std::string mystring;
            params_storage_file >> mystring;
            std::cout << std::setfill(' ') << std::setw(30) << mystring << " ";
        } else {
            std::string param_value;
            params_storage_file >> param_value;
            try {
                uint32_t int_param_value = std::stoi(param_value);
                memcpy(flash_memory + 4 * int_param_idx, &int_param_value, 4);
                std::cout << "(offset=" << 4 * int_param_idx << ") ";
                int_param_idx++;
            } catch (std::invalid_argument const& ex) {
                size_t offset =
                    PAGE_SIZE_BYTES - MAX_STRING_LENGTH * (1 + str_param_idx);
                memcpy(flash_memory + offset, param_value.c_str(),
                       MAX_STRING_LENGTH);
                std::cout << "(offset=" << offset << ") ";
                str_param_idx++;
            }
            std::cout << param_value << std::endl;
        }
        read_counter++;
    }
    std::cout << "flash is done" << std::endl;
    std::cout << std::endl;
}

void flashLoadBufferFromFile() {
// #if defined(FLASH_DRIVER_SIM_STORAGE_FILE) || defined(FLASH_DRIVER_STORAGE_FILE)
    std::ifstream params_storage_file;
    // #ifdef FLASH_DRIVER_SIM_STORAGE_FILE
    auto file_dir = FLASH_DRIVER_SIM_STORAGE_FILE;
    fs::directory_entry entry{file_dir};
    if (!entry.exists()) {
        file_dir = FLASH_DRIVER_STORAGE_FILE;
    }
    // #else
    auto file_dir = FLASH_DRIVER_STORAGE_FILE;
    // #endif
    params_storage_file.open(file_dir, std::ios::in);
    if (!params_storage_file) {
        std::cout << "Flash driver: " << file_dir << " been found!"
                  << std::endl;
    }
    size_t read_counter = 0;
    size_t int_param_idx = 0;
    size_t str_param_idx = 0;
    while (params_storage_file) {
        if (read_counter % 2 == 0) {
            std::string mystring;
            params_storage_file >> mystring;
            std::cout << std::setfill(' ') << std::setw(30) << mystring << " ";
        } else {
            std::string param_value;
            params_storage_file >> param_value;
            try {
                uint32_t int_param_value = std::stoi(param_value);
                memcpy(flash_memory + 4 * int_param_idx, &int_param_value, 4);
                std::cout << "(offset=" << 4 * int_param_idx << ") ";
                int_param_idx++;
            } catch (std::invalid_argument const& ex) {
                size_t offset =
                    PAGE_SIZE_BYTES - MAX_STRING_LENGTH * (1 + str_param_idx);
                memcpy(flash_memory + offset, param_value.c_str(),
                       MAX_STRING_LENGTH);
                std::cout << "(offset=" << offset << ") ";
                str_param_idx++;
            }
            std::cout << param_value << std::endl;
        }
        read_counter++;
    }
    std::cout << "flash is done" << std::endl;
    std::cout << std::endl;
#endif
}

void flashSaveBufferToFile() {
#ifdef FLASH_DRIVER_SIM_STORAGE_FILE
    std::cout << "Flash driver: save data to " << FLASH_DRIVER_SIM_STORAGE_FILE
              << "..." << std::endl;

    std::ofstream params_storage_file;
    params_storage_file.open(FLASH_DRIVER_SIM_STORAGE_FILE, std::ios::out);
    if (!params_storage_file) {
        std::cout << "Flash driver: " << FLASH_DRIVER_SIM_STORAGE_FILE
                  << " could not be opened for writing!" << std::endl;
        return;
    }

    size_t idx = 0;
    size_t str_idx = 0;

    for (size_t index = 0; index < NUM_OF_STR_PARAMS; index++) {
        std::string str_param_value(
            reinterpret_cast<char*>(flash_memory + index * MAX_STRING_LENGTH),
            MAX_STRING_LENGTH);
        std::cout << "(" << string_desc_pool[str_idx].name << "="
                  <<(str_param_value.c_str()) << ") " << std::endl;
        params_storage_file << string_desc_pool[str_idx].name << ": "
                            << str_param_value.c_str() << std::endl;
        str_idx++;
    }

    for (size_t index = 0; index < IntParamsIndexes::INTEGER_PARAMS_AMOUNT;
         index++) {
        uint32_t int_param_value;
        memcpy(&int_param_value, flash_memory + index * 4, 4);
        std::cout << "(" << integer_desc_pool[idx].name << "="
                  << int_param_value << ") " << std::endl;
        params_storage_file << integer_desc_pool[idx].name << ": "
                            << int_param_value << std::endl;
        idx++;
    }
    params_storage_file.close();
    std::cout << "Flash driver: data saved to " << FLASH_DRIVER_SIM_STORAGE_FILE
              << std::endl;
#endif
}
