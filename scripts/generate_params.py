#!/usr/bin/env python3
#
# Copyright (c) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.

"""Parameters generator."""

import os
import sys
from color_logging import log_err
import yaml
from params import IntegerParam, StringParam

LICENSE_HEADER = """// This file was automatically generated. Do not edit it manually.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/."""

class Generator:
    def __init__(self, directory, name) -> None:
        self.dir = directory
        self.name = name
        self.integers_array = ""
        self.integers_enums = ""
        self.strings_array = ""
        self.strings_enums = ""
        self.strings_amount = 0

    def add_integer(self, param : IntegerParam):
        assert isinstance(param, IntegerParam)

        c_string = (
            f"    {{{param.name :<32}, "
            f"{param.min}, "
            f"{param.max}, "
            f"{param.default}, "
            f"{param.mutability}, "
            f"{param.is_required}}},\n"
        )
        h_string = f"    {param.enum_name},\n"

        self.integers_array += c_string
        self.integers_enums += h_string

    def add_string(self, param : StringParam):
        assert isinstance(param, StringParam)
        c_string = f"    {{{param.name :<32}, {param.default}, {param.mutability}}},\n"
        h_string = f"    {param.enum_name},\n" if self.strings_amount > 0 else f"    {param.enum_name} = INTEGER_PARAMS_AMOUNT,\n"
        self.strings_array += c_string
        self.strings_enums += h_string
        self.strings_amount += 1

    def generate(self):
        if not os.path.exists(self.dir):
            os.makedirs(self.dir)
        with open(f"{self.dir}/{self.name}.cpp", 'w', encoding="utf-8") as cpp_file:
            cpp_content = (
                f"{LICENSE_HEADER}\n"
                "#include \"params.hpp\"\n"
                "\n"
                "IntegerDesc_t integer_desc_pool[] = {\n"
                f"{self.integers_array}"
                "};\n"
                "IntegerParamValue_t "
                "integer_values_pool[sizeof(integer_desc_pool) / sizeof(IntegerDesc_t)];\n"
                "\n"
                "StringDesc_t string_desc_pool[NUM_OF_STR_PARAMS] = {\n"
                f"{self.strings_array}"
                "};\n"
                "StringParamValue_t "
                "string_values_pool[sizeof(string_desc_pool) / sizeof(StringDesc_t)];\n"
            )
            cpp_file.write(cpp_content)

        with open(f"{self.dir}/{self.name}.hpp", 'w', encoding="utf-8") as hpp_file:
            hpp_content = (
                f"{LICENSE_HEADER}\n"
                "#pragma once\n"
                "#include \"storage.h\"\n\n"
                "enum IntParamsIndexes {\n"
                f"{self.integers_enums}\n"
                "    INTEGER_PARAMS_AMOUNT\n"
                "};\n"
                "enum StrParamsIndexes {\n"
                f"{self.strings_enums}\n"
                "};\n"
                f"#define NUM_OF_STR_PARAMS {self.strings_amount}\n"
            )
            hpp_file.write(hpp_content)

if __name__=="__main__":
    from argparse import ArgumentParser
    parser = ArgumentParser(description=__doc__)
    parser.add_argument("--out-dir",        type=str, required=True,    help="")
    parser.add_argument("--out-file-name",  type=str, default='params', help="")
    parser.add_argument("--language",       type=str, default="c++",    help="", choices=["c++"])
    parser.add_argument('-f','--files',     type=str, required=True,    help='', nargs='+')
    args = parser.parse_args()

    print("Parameters generator:")
    print("1. out_dir:", args.out_dir)
    print("2. language:", args.language)
    print("3. out-file-name:", args.out_file_name)
    print("4. files:", args.files)

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
