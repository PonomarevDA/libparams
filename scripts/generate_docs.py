#!/usr/bin/env python3
#
# Copyright (c) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.
"""
Generate README.md based on yaml files with parameters.
"""

import os
import sys
from argparse import ArgumentParser
from typing import Tuple
import yaml

from color_logging import log_err
from params import IntegerParam, StringParam

LANGUAGE_C = 0
LANGUAGE_CPP = 1

def port_data_type_to_md(origin_data_type : str) -> str:
    """
    Input example: uavcan.si.sample.voltage.Scalar.1.0
    Output example: [uavcan.si.sample.voltage.Scalar.1.0](
                    https://github.com/OpenCyphal/public_regulated_data_types/blob/master/
                    uavcan/si/sample/voltage/Scalar.1.0.dsdl)
    """
    assert isinstance(origin_data_type, str)
    splitted = origin_data_type.split('.')
    assert len(splitted) > 3, f"The {origin_data_type} is too short to be a data type."
    assert splitted[-1].isdigit() and splitted[-2].isdigit(), \
        f"The {origin_data_type} doesn't keep a version of dsdl."

    def is_uavcan_dsdl(splitted):
        return splitted[0] == 'uavcan'
    def is_udral_dsdl(splitted):
        return splitted[0] == 'reg' and splitted[1] == 'udral'

    if not is_uavcan_dsdl(splitted) and not is_udral_dsdl(splitted):
        return f"[{origin_data_type}]({origin_data_type})"

    splitted_data_type = splitted[0:-2]
    visible_version = f"{splitted[-2]}.{splitted[-1]}"
    visible_data_type = ".".join(splitted_data_type)
    link = (
        "https://github.com/OpenCyphal/public_regulated_data_types/blob/master/"
        f"{'/'.join(splitted_data_type)}.{visible_version}.dsdl")
    markdown_data_type = f"[{visible_data_type}]({link})"

    return markdown_data_type

def write_cyphal_topics(file, cyphal_topics : dict) -> None:
    assert isinstance(cyphal_topics, dict)

    file.write("| №  | Data type and topic name  | Description |\n")
    file.write("| -- | ------------------------- | ----------- |\n")
    counter = 1
    for port_name, port_info in cyphal_topics.items():
        data_type = port_data_type_to_md(port_info['data_type'])
        topic_name = port_name[11:]
        note = port_info.get('note')
        note = note.replace('\n', '</br>') if note is not None else ""
        file.write(f"| {counter :> 2} | {data_type} </br> {topic_name} | {note}|\n")
        counter += 1
    file.write("\n")

def write_parameters(file, all_params : list) -> None:
    assert isinstance(all_params, list)

    if len(all_params) >= 1:
        file.write("The node has the following registers:\n\n")
        file.write("| №  | Register name           | Description |\n")
        file.write("| -- | ----------------------- | ----------- |\n")

        counter = 1
        for param in all_params:
            param.name = param.name.replace('"', '')
            file.write(f"| {counter :> 2} | {param.name.ljust(23)} | {param.note} |\n")
            counter += 1
        file.write("\n")
    else:
        file.write("The node doesn't have registers:\n\n")

def generate_markdown_doc(cyphal_pubs : dict,
                          cyphal_subs : dict,
                          all_params : list,
                          output_markdown_filename : str = 'README.md') -> None:
    assert isinstance(cyphal_pubs, dict)
    assert isinstance(cyphal_subs, dict)
    assert isinstance(all_params, list)
    assert isinstance(output_markdown_filename, str)

    with open(output_markdown_filename, 'w', encoding="utf-8") as file:
        if len(cyphal_pubs) + len(cyphal_subs) >= 1:
            file.write("The node has the following interface:\n\n")

        if len(cyphal_pubs) >= 1:
            file.write("Cyphal Publishers:\n")
            write_cyphal_topics(file, cyphal_pubs)

        if len(cyphal_subs) >= 1:
            file.write("Cyphal Subscribers:\n")
            write_cyphal_topics(file, cyphal_subs)

        write_parameters(file, all_params)

        file.write("> This docs was automatically generated. Do not edit it manually.\n\n")

def parse_yaml_files(input_yaml_files : list) -> Tuple[dict, dict, list]:
    assert isinstance(input_yaml_files, list) and len(input_yaml_files) != 0

    all_params = []
    cyphal_pubs = {}
    cyphal_subs = {}
    for yaml_file in input_yaml_files:
        with open(yaml_file, "r", encoding="utf-8") as file_with_params:
            params = yaml.safe_load(file_with_params)

        for name, param_info in params.items():
            param_type = param_info.get('type', None)

            if param_type == 'Integer':
                all_params.append(IntegerParam.create(name, param_info))
            elif param_type == 'String':
                all_params.append(StringParam.create(name, param_info))
            elif param_type == 'Port':
                port_type = name[7:10]
                if port_type == "pub":
                    cyphal_pubs[name] = param_info
                elif port_type == "sub":
                    cyphal_subs[name] = param_info

    return cyphal_pubs, cyphal_subs, all_params

def main(input_yaml_files : list, output_markdown_filename = 'README.md'):
    assert isinstance(input_yaml_files, list) and len(input_yaml_files) != 0
    assert isinstance(output_markdown_filename, str)

    for yaml_file in input_yaml_files:
        if not os.path.exists(yaml_file):
            log_err(f"Input file with paths `{yaml_file}` is not exist!")
            sys.exit()

    cyphal_pubs, cyphal_subs, all_params = parse_yaml_files(input_yaml_files)
    generate_markdown_doc(cyphal_pubs, cyphal_subs, all_params, output_markdown_filename)

if __name__=="__main__":
    parser = ArgumentParser(description=__doc__)
    parser.add_argument('files', nargs='+', help='YAML files to process')
    args = parser.parse_args()

    main(input_yaml_files=sys.argv[1:])
