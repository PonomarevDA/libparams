#!/usr/bin/env python3
#
# Copyright (c) 2024 Anastasiia Stepanova <asiiapine@gmail.com>
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.

"""Default parameters generator."""

import math
import os
import sys
from color_logging import log_err
import yaml
from params import IntegerParam, StringParam

# hot-fix for a double quotes replacement 
double_quotes = '"'

class Generator:
    def __init__(self, directory, name) -> None:
        self.dir = directory
        self.name = name
        self.integers_array = []
        self.strings_array = []

    def add_integer(self, param : IntegerParam):
        assert isinstance(param, IntegerParam)
        c_string = f"{param.name.replace(double_quotes,'') :<32}:\t{param.default}\n"
        self.integers_array.append(c_string)

    def add_string(self, param : StringParam):
        assert isinstance(param, StringParam)
        c_string = f"{param.name.replace(double_quotes,'') :<32}:\t{param.default}\n"
        self.strings_array.append(c_string)

    def generate(self):
        if not os.path.exists(self.dir):
            os.makedirs(self.dir)
        array_size = 0
        page_size = 2048
        yaml_content = ""
        integer_iter = iter(self.integers_array)
        string_iter = iter(self.strings_array)

        num_pages = math.ceil((4 * len(self.integers_array) + 56 * len(self.strings_array)) 
                              / page_size)
        for page_idx in range(num_pages):
            with open(f"{self.dir}/{self.name}_{page_idx}.yml", 'w', encoding="utf-8") as yaml_fd:
                while array_size < page_size:
                    try:
                        yaml_content += next(integer_iter)
                        array_size += 4
                    except StopIteration:
                        try:
                            yaml_content += next(string_iter)
                            array_size += 56
                        except:
                            yaml_fd.write(yaml_content)
                            return
                array_size = 0
                yaml_fd.write(yaml_content)

if __name__=="__main__":
    from argparse import ArgumentParser
    parser = ArgumentParser(description=__doc__)
    parser.add_argument("--out-dir",        type=str, required=True,    help="")
    parser.add_argument("--out-file-name",  type=str, default='params', help="")
    parser.add_argument('-f','--files',     type=str, required=True,    help='', nargs='+')
    args = parser.parse_args()

    print("Parameters generator:")
    print("1. out_dir:", args.out_dir)
    print("2. out-file-name:", args.out_file_name)
    print("3. files:", args.files)

    # Check args for basic errors
    for yaml_file_path in args.files:
        if not os.path.exists(yaml_file_path):
            log_err(f"Input file with paths `{yaml_file_path}` is not exist!")
            sys.exit(1)

    gen = Generator(args.out_dir, args.out_file_name)

    for yaml_file_path in args.files:
        with open(yaml_file_path, "r", encoding="utf-8") as yaml_fd:
            params = yaml.safe_load(yaml_fd)
            for param_name in params:
                data = params[param_name]
                assert isinstance(data, dict), "Legacy style detected. Abort."
                if 'type' not in data:
                    log_err(f"Type is not exist: {param_name}!")
                    sys.exit(1)
                elif data['type'].lower() == "port":
                    gen.add_integer(IntegerParam.create_port_id(param_name, data['enum_base']))
                    gen.add_string(StringParam.create_port_type(param_name, data['data_type']))
                elif data['type'].lower() == "integer":
                    gen.add_integer(IntegerParam.create(param_name, data))
                elif data['type'].lower() == "string":
                    gen.add_string(StringParam.create(param_name, data))
                else:
                    log_err(f"Unknown type: {param_name}.type={data['type']}!")
                    sys.exit(1)

    gen.generate()
