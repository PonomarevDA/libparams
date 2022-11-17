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
} IntParamsIndexes;
"""

INTEGER_C_HEAD="""#include "params.hpp"
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

class Generator:
    def __init__(self, language, out_path, out_file_name) -> None:
        self.language = language
        self.path = out_path

        if language == LANGUAGE_C:
            self.out_source_file = f"{out_path}/{out_file_name}.c"
            self.out_header_file = f"{out_path}/{out_file_name}.h"
            self.int_header_head = INTEGER_H_HEAD
            self.int_source_head = INTEGER_C_HEAD
            self.int_header_tail = INTEGER_H_TAIL
        else:
            self.out_source_file = f"{out_path}/{out_file_name}.cpp"
            self.out_header_file = f"{out_path}/{out_file_name}.hpp"  
            self.int_header_head = INTEGER_HPP_HEAD
            self.int_source_head = INTEGER_CPP_HEAD
            self.int_header_tail = INTEGER_HPP_TAIL
        self.int_source_tail = INTEGER_C_CPP_TAIL

    def set_params(self, params):
        self.params = params

    def generate_head(self):
        out_cpp_fd = open(self.out_source_file, 'w')
        out_cpp_fd.write(self.int_source_head)
        out_cpp_fd.close()

        out_hpp_fd = open(self.out_header_file, 'w')
        out_hpp_fd.write(self.int_header_head)
        out_hpp_fd.close()

    def generate_tail(self):
        out_cpp_fd = open(self.out_source_file, 'a')
        out_cpp_fd.write(self.int_source_tail)
        out_cpp_fd.close()

        out_hpp_fd = open(self.out_header_file, 'a')
        out_hpp_fd.write(self.int_header_tail)
        out_hpp_fd.close()

    def process_integer_param(self, param_name):
        name = f"\"{param_name}\""
        enum_name = self.params[param_name][1]
        def_value = self.params[param_name][3]
        min_value = self.params[param_name][4]
        max_value = self.params[param_name][5]

        out_cpp_fd = open(self.out_source_file, "a")
        c_string = "    {}(uint8_t*){}, {}, {}, {}{},\n".format("{", name, min_value, max_value, def_value, "}")
        out_cpp_fd.write(c_string)
        out_cpp_fd.close()

        out_hpp_fd = open(self.out_header_file, "a")
        out_hpp_fd.write(f"    {enum_name},\n")
        out_hpp_fd.close()

    def process_param(self, param_name):
        param_type = self.params[param_name][0]
        if param_type == "Integer":
            self.process_integer_param(param_name)

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
        log_err(f"Language `{language}` not supported yet!")
        exit()
        # print("2. language", language)
        # language = LANGUAGE_C
    else:
        log_err(f"Unknown language `{language}`!")
        exit()

    out_file_name = sys.argv[3]
    print("3. out_file_name", out_file_name)

    params_generator = Generator(LANGUAGE_CPP, out_path, out_file_name)
    params_generator.generate_head()

    for yaml_file_idx in range(4, num_of_args):
        input_dir = sys.argv[yaml_file_idx]
        params_generator.process_yaml_file(input_dir)

    params_generator.generate_tail()
