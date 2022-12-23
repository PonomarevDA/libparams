#!/usr/bin/env python3
import os
import sys
from color_logging import log_err
import yaml

LANGUAGE_C = 0
LANGUAGE_CPP = 1

INTEGER_HPP_HEAD="""#pragma once
enum class IntParamsIndexes {
"""
INTEGER_HPP_TAIL="""
    INTEGER_PARAMS_AMOUNT
};
"""
INTEGER_H_HEAD="""#pragma once
typedef enum {
"""
INTEGER_H_TAIL="""
    INTEGER_PARAMS_AMOUNT
} IntParamsIndexes;
"""

INTEGER_C_HEAD="""#include "params.h"
#include "storage.h"
IntegerDesc_t integer_desc_pool[] = {
"""
INTEGER_CPP_HEAD="""#include "params.hpp"
extern "C" {
    #include "storage.h"
}
IntegerDesc_t integer_desc_pool[] = {
"""
INTEGER_C_CPP_TAIL="""
};
IntegerParamValue_t integer_values_pool[sizeof(integer_desc_pool) / sizeof(IntegerDesc_t)];
"""

STRING_HPP_HEAD="""#pragma once
"""
STRING_HPP_TAIL="""
"""
STRING_CPP_HEAD="""#include "string_params.hpp"
extern "C" {
    #include "storage.h"
}
StringDesc_t __attribute__((weak)) string_desc_pool[NUM_OF_STR_PARAMS] = {
"""
STRING_CPP_TAIL="""
};
StringParamValue_t string_values_pool[sizeof(string_desc_pool) / sizeof(StringDesc_t)];
"""

class Generator:
    def __init__(self, language, out_path, out_file_name) -> None:
        self.language = language
        self.path = out_path

        if language == LANGUAGE_C:
            self.out_int_source_file = f"{out_path}/{out_file_name}.c"
            self.out_int_header_file = f"{out_path}/{out_file_name}.h"
            self.int_header_head = INTEGER_H_HEAD
            self.int_source_head = INTEGER_C_HEAD
            self.int_header_tail = INTEGER_H_TAIL
        else:
            self.out_int_source_file = f"{out_path}/{out_file_name}.cpp"
            self.out_int_header_file = f"{out_path}/{out_file_name}.hpp"
            self.out_str_source_file = f"{out_path}/string_{out_file_name}.cpp"
            self.out_str_header_file = f"{out_path}/string_{out_file_name}.hpp"
            self.int_header_head = INTEGER_HPP_HEAD
            self.int_source_head = INTEGER_CPP_HEAD
            self.int_header_tail = INTEGER_HPP_TAIL
            self.str_header_head = STRING_HPP_HEAD
            self.str_source_head = STRING_CPP_HEAD
            self.str_source_tail = STRING_CPP_TAIL
            self.str_header_tail = STRING_HPP_TAIL
        self.int_source_tail = INTEGER_C_CPP_TAIL

        self.num_of_int_params = 0
        self.num_of_str_params = 0


    def set_params(self, params):
        self.params = params

    @staticmethod
    def open_and_write(file, text):
        fd = open(file, 'w')
        fd.write(text)
        fd.close()

    @staticmethod
    def open_and_append(file, text):
        fd = open(file, 'a')
        fd.write(text)
        fd.close()

    def generate_head(self):
        Generator.open_and_write(self.out_int_source_file, self.int_source_head)
        Generator.open_and_write(self.out_int_header_file, self.int_header_head)

        Generator.open_and_write(self.out_str_source_file, self.str_source_head)
        Generator.open_and_write(self.out_str_header_file, self.str_header_head)

    def generate_tail(self):
        Generator.open_and_append(self.out_int_source_file, self.int_source_tail)
        Generator.open_and_append(self.out_int_header_file, self.int_header_tail)

        Generator.open_and_append(self.out_str_source_file, self.str_source_tail)
        h_string = f"#define NUM_OF_STR_PARAMS {self.num_of_str_params}"
        Generator.open_and_append(self.out_str_header_file, h_string)
        Generator.open_and_append(self.out_str_header_file, self.str_header_tail)

    def process_integer_param(self, param_name):
        name = f"\"{param_name}\""
        enum_name = self.params[param_name][1]
        def_value = self.params[param_name][3]
        min_value = self.params[param_name][4]
        max_value = self.params[param_name][5]

        c_string = "    {}(uint8_t*){}, {}, {}, {}{},\n".format("{", name, min_value, max_value, def_value, "}")
        Generator.open_and_append(self.out_int_source_file, c_string)

        h_string = f"    {enum_name},\n"
        Generator.open_and_append(self.out_int_header_file, h_string)

    def process_string_param(self, param_name):
        name = f"\"{param_name}\""
        # enum_name = self.params[param_name][1]
        is_mutable = str(self.params[param_name][2]).lower()
        def_value = "\"{}\"".format(self.params[param_name][3])

        c_string = "    {}(uint8_t*){}, {}, {}{},\n".format("{", name, def_value, is_mutable, "}")
        Generator.open_and_append(self.out_str_source_file, c_string)
        self.num_of_str_params += 1

    def process_param(self, param_name):
        param_type = self.params[param_name][0]
        if param_type == "Integer":
            self.process_integer_param(param_name)
        elif param_type == "String":
            self.process_string_param(param_name)

    def process_yaml_file(self, input_dir):
        if not os.path.exists(input_dir):
            log_err(f"Input file with paths `{input_dir}` is not exist!")
            exit()
        file_with_params = open(input_dir, "r")
        params = yaml.safe_load(file_with_params)
        params_generator.set_params(params)
        for param_name in params:
            params_generator.process_param(param_name)

if __name__=="__main__":
    num_of_args = len(sys.argv)
    if num_of_args <= 4:
        log_err("Wrong args number")
        exit()

    out_path = sys.argv[1]
    if not os.path.exists(out_path):
        log_err(f"Output path `{out_path}` is not exist!")
        exit()
    else:
        print("1. out_path", out_path)

    language = sys.argv[2]
    if language == "c++":
        print("2. language", language)
        language = LANGUAGE_CPP
    elif language == "c":
        print("2. language", language)
        language = LANGUAGE_C
    else:
        log_err(f"Unknown language `{language}`!")
        exit()

    out_file_name = sys.argv[3]
    print("3. out_file_name", out_file_name)

    params_generator = Generator(language, out_path, out_file_name)
    params_generator.generate_head()

    for yaml_file_idx in range(4, num_of_args):
        input_dir = sys.argv[yaml_file_idx]
        params_generator.process_yaml_file(input_dir)

    params_generator.generate_tail()
