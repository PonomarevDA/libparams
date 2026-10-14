#!/usr/bin/env python3
#
# Copyright (c) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
#                    Anastasiia Stepanova <asiiapine@gmail.com>
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.

"""Generate the C/C++ parameter tables and the SITL default values from YAML files."""

import logging
import math
import os
import sys
from argparse import ArgumentParser
from typing import List
import yaml
from params import IntegerParam, StringParam, ParamsError, load_params

LICENSE_HEADER = """// This file was automatically generated. Do not edit it manually.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/."""

# Must match the SITL flash driver: platform_specific/ubuntu/flash_driver.cpp, storage.h.
PAGE_SIZE = 2048
INTEGER_SIZE = 4
STRING_SIZE = 56

EXTENSIONS = {"c++": ("cpp", "hpp"), "c": ("c", "h")}

logger = logging.getLogger("libparams-gen")


def render_source(integers: List[IntegerParam], strings: List[StringParam],
                  header_name: str) -> str:
    integers_array = "".join(
        f"    {{{quote(param.name) :<32}, {param.min}, {param.max}, {param.default}, "
        f"{param.mutability}, {param.is_required}}},\n" for param in integers)
    strings_array = "".join(
        f"    {{{quote(param.name) :<32}, {quote(param.default)}, {param.mutability}}},\n"
        for param in strings)
    return (
        f"{LICENSE_HEADER}\n"
        f"#include \"{header_name}\"\n"
        "\n"
        "extern const IntegerDesc_t integer_desc_pool[];\n"
        "const IntegerDesc_t integer_desc_pool[] = {\n"
        f"{integers_array}"
        "};\n"
        "IntegerParamValue_t "
        "integer_values_pool[sizeof(integer_desc_pool) / sizeof(IntegerDesc_t)];\n"
        "\n"
        "extern const StringDesc_t string_desc_pool[NUM_OF_STR_PARAMS];\n"
        "const StringDesc_t string_desc_pool[NUM_OF_STR_PARAMS] = {\n"
        f"{strings_array}"
        "};\n"
        "StringParamValue_t "
        "string_values_pool[sizeof(string_desc_pool) / sizeof(StringDesc_t)];\n"
    )


def render_header(integers: List[IntegerParam], strings: List[StringParam]) -> str:
    integers_enums = "".join(f"    {param.enum_name},\n" for param in integers)
    content = (
        f"{LICENSE_HEADER}\n"
        "#pragma once\n"
        "#include \"storage.h\"\n\n"
        "enum IntParamsIndexes {\n"
        f"{integers_enums}\n"
        "    INTEGER_PARAMS_AMOUNT\n"
        "};\n"
    )
    if strings:
        strings_enums = f"    {strings[0].enum_name} = INTEGER_PARAMS_AMOUNT,\n"
        strings_enums += "".join(f"    {param.enum_name},\n" for param in strings[1:])
        content += (
            "enum StrParamsIndexes {\n"
            f"{strings_enums}\n"
            "};\n"
        )
    content += f"#define NUM_OF_STR_PARAMS {len(strings)}\n"
    return content


def render_default_params(integers: List[IntegerParam],
                          strings: List[StringParam]) -> List[str]:
    """One YAML file per flash page, split the way the SITL driver writes its nvm files."""
    params_size = INTEGER_SIZE * len(integers) + STRING_SIZE * len(strings)
    num_pages = max(1, math.ceil(params_size / PAGE_SIZE))
    pages = []
    int_idx = 0
    str_idx = 0
    for _ in range(num_pages):
        content = ""
        used = 0
        while int_idx < len(integers) and used + INTEGER_SIZE <= PAGE_SIZE:
            content += f"{integers[int_idx].name :<32}: {integers[int_idx].default}\n"
            used += INTEGER_SIZE
            int_idx += 1
        if int_idx == len(integers):
            while str_idx < len(strings) and used + STRING_SIZE <= PAGE_SIZE:
                content += f"{strings[str_idx].name :<32}: {quote(strings[str_idx].default)}\n"
                used += STRING_SIZE
                str_idx += 1
        pages.append(content)
    return pages


def quote(value) -> str:
    return f"\"{value}\""


def write_if_changed(path: str, content: str) -> str:
    if os.path.exists(path):
        with open(path, 'r', encoding="utf-8") as existing_file:
            if existing_file.read() == content:
                return "unchanged"
        status = "updated"
    else:
        status = "added"
    with open(path, 'w', encoding="utf-8") as out_file:
        out_file.write(content)
    return status


def write_code(out_dir: str, name: str, language: str,
               integers: List[IntegerParam], strings: List[StringParam]) -> None:
    os.makedirs(out_dir, exist_ok=True)
    source_ext, header_ext = EXTENSIONS[language]
    source = f"{name}.{source_ext}"
    header = f"{name}.{header_ext}"
    source_status = write_if_changed(os.path.join(out_dir, source),
                                     render_source(integers, strings, header))
    header_status = write_if_changed(os.path.join(out_dir, header),
                                     render_header(integers, strings))
    logger.info("%s %s / %s %s", source, source_status, header, header_status)


def write_default_params(out_dir: str, name: str,
                         integers: List[IntegerParam], strings: List[StringParam]) -> None:
    os.makedirs(out_dir, exist_ok=True)
    pages = render_default_params(integers, strings)
    for page_idx, content in enumerate(pages):
        write_if_changed(os.path.join(out_dir, f"{name}_{page_idx}.yaml"), content)
    logger.info("%s: %d page(s) in %s", name, len(pages), out_dir)


def setup_logger() -> None:
    if not logger.handlers:
        handler = logging.StreamHandler()
        handler.setFormatter(logging.Formatter("[libparams-gen] [%(levelname)s] %(message)s"))
        logger.addHandler(handler)
    logger.setLevel(logging.INFO)


def main() -> int:
    parser = ArgumentParser(description=__doc__)
    parser.add_argument("--out-dir", type=str, required=True,
                        help="directory for the generated source and header")
    parser.add_argument("--out-file-name", type=str, default='params',
                        help="base name of the generated source and header")
    parser.add_argument("--language", type=str, default="c++", choices=list(EXTENSIONS))
    parser.add_argument('-f', '--files', type=str, required=True, nargs='+',
                        help="YAML files with parameters, in index order")
    parser.add_argument("--default-params-dir", type=str,
                        help="also write SITL default values <name>_<page>.yaml here")
    parser.add_argument("--default-params-name", type=str, default="default_params",
                        help="base name of the SITL default values files")
    args = parser.parse_args()

    setup_logger()
    logger.info("Generating params: out_dir=%s, out_file=%s, files=%d",
                args.out_dir, args.out_file_name, len(args.files))
    try:
        integers, strings = load_params(args.files)
    except (OSError, yaml.YAMLError, ParamsError) as err:
        logger.error("%s", err)
        return 1

    write_code(args.out_dir, args.out_file_name, args.language, integers, strings)
    if args.default_params_dir:
        write_default_params(args.default_params_dir, args.default_params_name,
                             integers, strings)
    return 0


if __name__ == "__main__":
    sys.exit(main())
