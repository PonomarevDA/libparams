#!/usr/bin/env python3
#
# Copyright (c) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.

import os
import sys
from color_logging import log_err
import yaml
from _constants import CppHeader, CHeader, CSource, CppSource
from params import IntegerParam, StringParam

LANGUAGE_C = 0
LANGUAGE_CPP = 1

class Generator:
    def __init__(self, language, out_path, out_file_name) -> None:
        self.language = language
        self.path = out_path

        if language == LANGUAGE_C:
            self.out_int_source_file = f"{out_path}/{out_file_name}.c"
            self.out_int_header_file = f"{out_path}/{out_file_name}.h"
            self.out_str_source_file = f"{out_path}/string_{out_file_name}.c"
            self.out_str_header_file = f"{out_path}/string_{out_file_name}.h"
            self.header = CHeader
            self.source = CSource
        else:
            self.out_int_source_file = f"{out_path}/{out_file_name}.cpp"
            self.out_int_header_file = f"{out_path}/{out_file_name}.hpp"
            self.out_str_source_file = f"{out_path}/string_{out_file_name}.cpp"
            self.out_str_header_file = f"{out_path}/string_{out_file_name}.hpp"
            self.header = CppHeader
            self.source = CppSource

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
        Generator.open_and_write(self.out_int_source_file, self.source.INTEGER_HEAD)
        Generator.open_and_write(self.out_int_header_file, self.header.INTEGER_HEAD)

        Generator.open_and_write(self.out_str_source_file, self.source.STRING_HEAD)
        Generator.open_and_write(self.out_str_header_file, self.header.STRING_HEAD)

    def generate_tail(self):
        Generator.open_and_append(self.out_int_source_file, self.source.INTEGER_TAIL)
        Generator.open_and_append(self.out_int_header_file, self.header.INTEGER_TAIL)

        Generator.open_and_append(self.out_str_source_file, self.source.STRING_TAIL)
        h_string = f"#define NUM_OF_STR_PARAMS {self.num_of_str_params}"
        Generator.open_and_append(self.out_str_header_file, h_string)
        Generator.open_and_append(self.out_str_header_file, self.header.STRING_TAIL)

    def append_integer(self, param : IntegerParam):
        c_string = f"    {{{param.name :<32}, {param.min}, {param.max}, {param.default}, MUTABLE}},\n"
        Generator.open_and_append(self.out_int_source_file, c_string)

        h_string = f"    {param.enum_name},\n"
        Generator.open_and_append(self.out_int_header_file, h_string)

    def append_string(self, param : StringParam):
        c_string = f"    {{{param.name :<32}, {param.default}, {param.mutability}}},\n"
        Generator.open_and_append(self.out_str_source_file, c_string)
        self.num_of_str_params += 1

    def process_param(self, param_name : str, data):
        if type(data) is dict:
            if param_name.startswith(("uavcan.sub.", "uavcan.pub.", "uavcan.cln.", "uavcan.srv.")) and not param_name.endswith((".id", ".type")):
                self.append_integer(IntegerParam.create_port_id(param_name, enum_base=data['enum_base']))
                self.append_string(StringParam.create_port_type(param_name, data_type=data['data_type']))
            elif data['type'] == "Integer":
                self.append_integer(IntegerParam.create(param_name, data))
            elif data['type'] == "String":
                self.append_string(StringParam.create(param_name, data))
        elif type(data) is list:
            param_type = data[0]
            if param_name.startswith(("uavcan.sub.", "uavcan.pub.", "uavcan.cln.", "uavcan.srv.")) and not param_name.endswith((".id", ".type")):
                self.append_integer(IntegerParam.create_port_id(param_name, enum_base=data[1]))
                self.append_string(StringParam.create_port_type(param_name, data_type=data[0]))
            elif param_type == "Integer":
                self.append_integer(IntegerParam.create(param_name, data))
            elif param_type == "String":
                self.append_string(StringParam.create(param_name, data))
            else:
                log_err(f"Can't parse string: {param_name} : {self.params[param_name]}")
        else:
            log_err(f"{param_name}, {type(data)}")

    def process_yaml_file(self, input_dir):
        if not os.path.exists(input_dir):
            log_err(f"Input file with paths `{input_dir}` is not exist!")
            exit()
        file_with_params = open(input_dir, "r")
        params = yaml.safe_load(file_with_params)
        params_generator.set_params(params)
        for param_name in params:
            params_generator.process_param(param_name, params[param_name])

def print_usage_help():
    log_err(f"Usage: {sys.argv[0]} <out_path> <language> <out_file_name> <input_dir>")

if __name__=="__main__":
    num_of_args = len(sys.argv)
    if num_of_args <= 4:
        print_usage_help()
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
