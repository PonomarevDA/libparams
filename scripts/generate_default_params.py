#!/usr/bin/env python3
#
# Copyright (c) 2024 Anastasiia Stepanova <asiiapine@gmail.com>
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.

"""Write SITL default values from a parameters source file.

Deprecated for generated parameters: generate_params.py --default-params-dir reads the YAML
files directly. This script stays for existing builds and for hand-written C parameters.
"""

import re
import sys
from argparse import ArgumentParser
from typing import List, Tuple
from params import IntegerParam, StringParam
from generate_params import logger, setup_logger, write_default_params

INTEGER_DESC = re.compile(r'\{\s*"([^"]*)"\s*,\s*([^,]+),\s*([^,]+),\s*([^,}]+)')
STRING_DESC = re.compile(r'\{\s*"([^"]*)"\s*,\s*"([^"]*)"')


def parse_params_source(path: str) -> Tuple[List[IntegerParam], List[StringParam]]:
    """Integer descriptors come before the integer values pool, string ones after it."""
    integers = []
    strings = []
    is_integer_desc_pool = True
    with open(path, "r", encoding="utf-8") as source:
        for line in source:
            if "IntegerParamValue_t" in line:
                is_integer_desc_pool = False
            if is_integer_desc_pool:
                match = INTEGER_DESC.search(line)
                if match:
                    integers.append(IntegerParam(name=match[1], default=match[4].strip()))
            else:
                match = STRING_DESC.search(line)
                if match:
                    strings.append(StringParam(name=match[1], default=match[2]))
    return integers, strings


def main() -> int:
    parser = ArgumentParser(description=__doc__)
    parser.add_argument("--out-dir", type=str, required=True)
    parser.add_argument("--out-file-name", type=str, default='params')
    parser.add_argument('-f', '--files', type=str, required=True, nargs='+',
                        help="params.c or params.cpp; only the last one is used")
    args = parser.parse_args()

    setup_logger()
    try:
        integers, strings = parse_params_source(args.files[-1])
    except OSError as err:
        logger.error("%s", err)
        return 1
    write_default_params(args.out_dir, args.out_file_name, integers, strings)
    return 0


if __name__ == "__main__":
    sys.exit(main())
