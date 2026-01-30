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
import logging
from argparse import ArgumentParser
from typing import Tuple
import yaml

from params import IntegerParam, StringParam

LOG_PREFIX = "[libparams-doc]"

logger = logging.getLogger(__name__)

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

    file.write("| Data type and topic name  | Description |\n")
    file.write("| ------------------------- | ----------- |\n")
    for port_name, port_info in cyphal_topics.items():
        data_type = port_data_type_to_md(port_info['data_type'])
        topic_name = port_name[11:]
        note = port_info.get('note')
        note = note.replace('\n', '</br>') if note is not None else ""
        file.write(f"| {data_type} </br> {topic_name} | {note}|\n")
    file.write("\n")

def write_parameters(file, all_params : list) -> None:
    assert isinstance(all_params, list)

    if len(all_params) >= 1:
        file.write("The node has the following registers:\n\n")
        file.write("| Register name           | Description |\n")
        file.write("| ----------------------- | ----------- |\n")

        for param in all_params:
            param.name = param.name.replace('"', '')
            file.write(f"| {param.name.ljust(23)} | {param.note} |\n")
        file.write("\n")
    else:
        file.write("The node doesn't have registers:\n\n")

def _write_if_changed(path, content: str) -> str:
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

def generate_markdown_doc(cyphal_pubs : dict,
                          cyphal_subs : dict,
                          all_params : list,
                          output_markdown_filename : str) -> None:
    assert isinstance(cyphal_pubs, dict)
    assert isinstance(cyphal_subs, dict)
    assert isinstance(all_params, list)
    assert isinstance(output_markdown_filename, str)

    content = ""
    if len(cyphal_pubs) + len(cyphal_subs) >= 1:
        content += "The node has the following interface:\n\n"

    if len(cyphal_pubs) >= 1:
        content += "Cyphal Publishers:\n"
        content += _render_cyphal_topics(cyphal_pubs)

    if len(cyphal_subs) >= 1:
        content += "Cyphal Subscribers:\n"
        content += _render_cyphal_topics(cyphal_subs)

    content += _render_parameters(all_params)

    content += "> This docs was automatically generated. Do not edit it manually.\n\n"
    return _write_if_changed(output_markdown_filename, content)

def _render_cyphal_topics(cyphal_topics: dict) -> str:
    assert isinstance(cyphal_topics, dict)

    content = "| Data type and topic name  | Description |\n"
    content += "| ------------------------- | ----------- |\n"
    for port_name, port_info in cyphal_topics.items():
        data_type = port_data_type_to_md(port_info['data_type'])
        topic_name = port_name[11:]
        note = port_info.get('note')
        note = note.replace('\n', '</br>') if note is not None else ""
        content += f"| {data_type} </br> {topic_name} | {note}|\n"
    content += "\n"
    return content

def _render_parameters(all_params : list) -> str:
    assert isinstance(all_params, list)

    content = ""
    if len(all_params) >= 1:
        content += "The node has the following registers:\n\n"
        content += "| Register name           | Description |\n"
        content += "| ----------------------- | ----------- |\n"

        for param in all_params:
            param.name = param.name.replace('"', '')
            content += f"| {param.name.ljust(23)} | {param.note} |\n"
        content += "\n"
    else:
        content += "The node doesn't have registers:\n\n"
    return content

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

def parse_yaml_files_and_generate_doc(input_yaml_files : list, output_markdown_path: str):
    assert isinstance(input_yaml_files, list) and len(input_yaml_files) != 0
    assert isinstance(output_markdown_path, str)

    for yaml_file in input_yaml_files:
        if not os.path.exists(yaml_file):
            logger.error("Input file with paths %s is not exist!", yaml_file)
            sys.exit(1)

    directory = os.path.dirname(output_markdown_path)
    if not os.path.exists(directory):
        os.makedirs(directory)

    cyphal_pubs, cyphal_subs, all_params = parse_yaml_files(input_yaml_files)
    return generate_markdown_doc(cyphal_pubs, cyphal_subs, all_params, output_markdown_path)

if __name__=="__main__":
    logging.basicConfig(level=logging.INFO, format=f"{LOG_PREFIX} [%(levelname)s] %(message)s")
    logger.setLevel(logging.INFO)

    parser = ArgumentParser(description=__doc__)
    parser.add_argument('files', nargs='+', help='YAML files to process')
    parser.add_argument("--output", type=str, required=False, default='README.md')
    args = parser.parse_args()

    logger.info("Generating docs: output=%s, files=%d", args.output, len(args.files))
    status = parse_yaml_files_and_generate_doc(args.files, args.output)
    logger.info("Docs %s", status)
