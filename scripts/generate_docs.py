#!/usr/bin/env python3
import os
import sys
from color_logging import log_err
import yaml
from params import IntegerParam, StringParam

LANGUAGE_C = 0
LANGUAGE_CPP = 1

all_params = []
all_ports = {}

if __name__=="__main__":
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
            f.write("| №  | Type | Message | Topic name  |\n")
            f.write("| -- | ---- | ------- | ----------- |\n")

            counter = 1
            for port_name, port_info in all_ports.items():
                port_type = port_name[7:10]
                port_data_type = port_info['data_type']
                topic_name = port_name[11:]
                f.write(f"| {counter :> 3} | {port_type} | {port_data_type} | {topic_name} | {port_info}|\n")
                counter += 1
            f.write("\n")

        if len(all_params) >= 1:
            f.write("The node has the following registers:\n\n")
            f.write("| №  | Register name           | Description |\n")
            f.write("| -- | ----------------------- | ----------- |\n")

            counter = 1
            for param in all_params:
                param.name = param.name.replace('"', '')
                f.write(f"|{counter :> 3} | {param.name.ljust(23)} | {param.note} |\n")
                counter += 1
            f.write("\n")
        else:
            f.write("The node doesn't have registers:\n\n")

        f.write(f"> This docs was automatically generated. Do not edit it manually.\n\n")
