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

class ParamsLoader:
    def __init__(self, directory) -> None:
        self.dir = directory
        self.integers_array: list[IntegerParam] = []
        self.strings_array: list[StringParam] = []

    def parse_params(self):
        with open(self.dir, "r", encoding="utf-8") as cpp_fd:
            line = None
            is_integer_desc_pool = True
            for line in cpp_fd:
                if "IntegerParamValue_t" in line:
                    is_integer_desc_pool = False
                if all(char in line for char in['{', '}', ',']):
                    line = line.replace('{', '').replace('}', '')
                    data = line.split(',')
                    if is_integer_desc_pool:
                        self.integers_array.append(IntegerParam(data[0].replace('    ', '').replace(double_quotes,''), default=data[3]))
                    else:
                        self.strings_array.append(StringParam(name=data[0].replace('    ', '').replace(double_quotes,''), default=data[1][1:]))

class Generator:
    def __init__(self, directory, name,
                 integers_array: list[IntegerParam] = [], strings_array: list[StringParam] = []) -> None:
        self.dir = directory
        self.name = name
        self.integers_array = integers_array
        self.strings_array = strings_array

    def generate(self):
        if not os.path.exists(self.dir):
            os.makedirs(self.dir)
        array_size = 0
        page_size = 2048
        integer_iter = iter(self.integers_array)
        string_iter = iter(self.strings_array)
        num_pages = math.ceil((4 * len(self.integers_array) + 56 * len(self.strings_array)) 
                              / page_size)

        for page_idx in range(num_pages):
            yaml_content = ""
            with open(f"{self.dir}/{self.name}_{page_idx}.yml", 'w', encoding="utf-8") as yaml_fd:
                while array_size < page_size - 56:
                    param = None
                    try:
                        param = next(integer_iter)
                        yaml_content += f"{param.name :<32}:{param.default}\n"
                        array_size += 4
                    except StopIteration:
                        try:
                            param = next(string_iter)
                            yaml_content += f'{param.name :<32}: "{param.default.replace('"', '')}"\n'
                            array_size += 56
                        except:
                            yaml_fd.write(yaml_content)
                            return
                array_size = 0
                yaml_fd.write(yaml_content)
                yaml_fd.close()

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
    print("3. cpp geneated param description file:", args.files)

    # Check args for basic errors
    for cpp_file_path in args.files:
        if not os.path.exists(cpp_file_path):
            log_err(f"Input file with paths `{cpp_file_path}` is not exist!")
            sys.exit(1)

    loader = ParamsLoader(cpp_file_path)
    loader.parse_params()
    gen = Generator(args.out_dir, args.out_file_name, loader.integers_array, loader.strings_array)

    gen.generate()
