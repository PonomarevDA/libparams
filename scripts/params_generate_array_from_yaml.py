#!/usr/bin/env python3
import os
import sys
from color_logging import log_warn, log_err
import yaml
from params import CppHeader, CHeader, CSource, CppSource
from dataclasses import dataclass

LANGUAGE_C = 0
LANGUAGE_CPP = 1

@dataclass
class IntegerParam:
    name: str = ""
    enum_name: str = ""
    flags: str = ""
    default: int = 0
    min: int = 0
    max: int = 0

    @staticmethod
    def create_from_dict(param_name, data : dict):
        Generator.is_mutable(param_name, str(data['flags']))
        integer_parameter = IntegerParam(
            name=f"\"{param_name}\"",
            flags="",
            enum_name=data['enum'],
            default=data['default'],
            min=data['min'],
            max=data['max'],
        )
        return integer_parameter

    @staticmethod
    def create_from_list_legacy(param_name, data : list):
        assert(type(data) is list)
        """Input example: param_name : ["Integer", "ENUM_NAME", "mutable", -1, -1, 31]"""
        Generator.is_mutable(param_name, str(data[2]))
        integer_parameter = IntegerParam(
            name=f"\"{param_name}\"",
            flags="",
            enum_name=data[1],
            default=data[3],
            min=data[4],
            max=data[5],
        )
        return integer_parameter

    @staticmethod
    def create_cyphal_port_id(param_name, enum_base : str):
        id_register = IntegerParam(
            name=f"\"{param_name}.id\"",
            flags="",
            enum_name=f"{enum_base}_ID",
            default=65535,
            min=0,
            max=65535,
        )
        return id_register

@dataclass
class StringParam:
    name: str = ""
    default: str = ""
    mutability: str = "IMMUTABLE"

    @staticmethod
    def create_from_dict(param_name, data : dict):
        string_param = StringParam(
            name=f"\"{param_name}\"",
            default="\"{}\"".format(data['default']),
            mutability=Generator.is_mutable(param_name, str(data['flags']))
        )
        return string_param

    @staticmethod
    def create_from_list_legacy(param_name, data : list):
        """Input example: param_name : ["data_type",  "ENUM_NAME"]"""
        string_param = StringParam(
            name=f"\"{param_name}\"",
            default="\"{}\"".format(data[3]),
            mutability=Generator.is_mutable(param_name, str(data[2]))
        )
        return string_param

    @staticmethod
    def create_cyphal_port_type(param_name, data_type : str):
        type_register = StringParam(
            name=f"\"{param_name}.type\"",
            default=f"\"{data_type}\"",
            mutability="IMMUTABLE"
        )
        return type_register

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
        c_string = "    {}{}, {}, {}, {}{},\n".format("{", param.name, param.min, param.max, param.default, "}")
        Generator.open_and_append(self.out_int_source_file, c_string)

        h_string = f"    {param.enum_name},\n"
        Generator.open_and_append(self.out_int_header_file, h_string)

    def append_string(self, param : StringParam):
        c_string = "    {}{}, {}, {}{},\n".format("{", param.name, param.default, param.mutability, "}")
        Generator.open_and_append(self.out_str_source_file, c_string)
        self.num_of_str_params += 1

    @staticmethod
    def is_mutable(param_name : str, mutability_str : str):
        name = f"\"{param_name}\""
        if mutability_str in ["True", "immutable"]:
            mutability = "IMMUTABLE"
        elif mutability_str in ["False", "mutable"]:
            mutability = "MUTABLE"
        else:
            log_err(f"Mutability of {name} can't be determined: {mutability_str} ({type(mutability_str)})")
            exit()
        if mutability_str in ["True", "False"]:
            log_warn(f"Mutability of {name} will be deprecated soon: {mutability_str}. Use `mutable` or `immutable`.")

        return mutability

    def process_param(self, param_name : str, data):
        if type(data) is list:
            param_type = data[0]
            if param_name.startswith(("uavcan.sub.", "uavcan.pub.", "uavcan.cln.", "uavcan.srv.")) and not param_name.endswith((".id", ".type")):
                self.append_integer(IntegerParam.create_cyphal_port_id(param_name, enum_base=data[1]))
                self.append_string(StringParam.create_cyphal_port_type(param_name, data_type=data[0]))
            elif param_type == "Integer":
                self.append_integer(IntegerParam.create_from_list_legacy(param_name, data))
            elif param_type == "String":
                self.append_string(StringParam.create_from_list_legacy(param_name, data))
            else:
                log_err(f"Can't parse string: {param_name} : {self.params[param_name]}")
        elif type(data) is dict:
            if param_name.startswith(("uavcan.sub.", "uavcan.pub.", "uavcan.cln.", "uavcan.srv.")) and not param_name.endswith((".id", ".type")):
                self.append_integer(IntegerParam.create_cyphal_port_id(param_name, enum_base=data['enum_base']))
                self.append_string(StringParam.create_cyphal_port_type(param_name, data_type=data['data_type']))
            elif data['type'] == "Integer":
                self.append_integer(IntegerParam.create_from_dict(param_name, data))
            elif data['type'] == "String":
                self.append_integer(StringParam.create_from_dict(param_name, data))
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
