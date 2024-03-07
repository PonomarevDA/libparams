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
from params import IntegerParam, StringParam

def port_data_type_to_md(origin_data_type : str) -> str:
    """
    Input example: uavcan.si.sample.voltage.Scalar.1.0
    Output example: [uavcan.voltage.Scalar.1.0](https://github.com/OpenCyphal/
                    public_regulated_data_types/blob/master/uavcan/si/sample/
                    voltage/Scalar.1.0.dsdl)
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

LANGUAGE_C = 0
LANGUAGE_CPP = 1

all_params = []
all_ports = {}

if __name__=="__main__":
    print(port_data_type_to_md("uavcan.si.sample.voltage.Scalar.1.0"))

    num_of_args = len(sys.argv)
    for yaml_file_idx in range(1, num_of_args):
        input_dir = sys.argv[yaml_file_idx]
        if not os.path.exists(input_dir):
            log_err(f"Input file with paths `{input_dir}` is not exist!")
            exit()
        file_with_params = open(input_dir, "r")
        params = yaml.safe_load(file_with_params)
        for param_name in params:
            if type(params[param_name]) is not dict:
                log_err(f"{param_name} has a legacy style. Support is deprecated.")
                continue
            if "type" not in params[param_name]:
                log_err(f"{param_name} has not type.")
            elif params[param_name]['type'] == 'Integer':
                all_params.append(IntegerParam.create(param_name, params[param_name]))
            elif params[param_name]['type'] == 'String':
                all_params.append(StringParam.create(param_name, params[param_name]))
            elif params[param_name]['type'] == 'Port':
                all_ports[param_name] = params[param_name]

    for param in all_params:
        if not hasattr(param, "note"):
            param.note = ""

    with open('README.md', 'w') as f:
        if len(all_ports) >= 1:
            f.write("The node has the following interface:\n\n")

            pubs = {}
            subs = {}
            for port_name, port_info in all_ports.items():
                port_type = port_name[7:10]
                if port_type == "pub":
                    pubs[port_name] = port_info
                elif port_type == "sub":
                    subs[port_name] = port_info

            f.write("Publishers:\n")
            f.write("| №  | Data type and topic name  | Description |\n")
            f.write("| -- | ------------------------- | ----------- |\n")
            counter = 1
            for port_name, port_info in pubs.items():
                data_type = port_data_type_to_md(port_info['data_type'])
                topic_name = port_name[11:]
                note = port_info.get('note')
                note = note.replace('\n', '</br>') if note is not None else ""
                f.write(f"| {counter :> 2} | {data_type} </br> {topic_name} | {note}|\n")
                counter += 1
            f.write("\n")

            f.write("Subscribers:\n")
            f.write("| №  | Data type and topic name | Description |\n")
            f.write("| -- | ------------------------ | ----------- |\n")
            counter = 1
            for port_name, port_info in subs.items():
                data_type = port_data_type_to_md(port_info['data_type'])
                topic_name = port_name[11:]
                note = port_info.get('note')
                note = note.replace('\n', '</br>') if note is not None else ""
                f.write(f"| {counter :> 2} | {data_type} </br> {topic_name} | {note}|\n")
                counter += 1
            f.write("\n")

        if len(all_params) >= 1:
            f.write("The node has the following registers:\n\n")
            f.write("| №  | Register name           | Description |\n")
            f.write("| -- | ----------------------- | ----------- |\n")

            counter = 1
            for param in all_params:
                param.name = param.name.replace('"', '')
                f.write(f"| {counter :> 2} | {param.name.ljust(23)} | {param.note} |\n")
                counter += 1
            f.write("\n")
        else:
            f.write("The node doesn't have registers:\n\n")

        f.write(f"> This docs was automatically generated. Do not edit it manually.\n\n")
