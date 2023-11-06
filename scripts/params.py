#!/usr/bin/env python3
from dataclasses import dataclass
from color_logging import log_warn, log_err


@dataclass
class IntegerParam:
    name: str = ""
    enum_name: str = ""
    flags: str = ""
    default: int = 0
    min: int = 0
    max: int = 0

    @staticmethod
    def create(param_name, data):
        if type(data) is dict:
            integer_parameter = IntegerParam._create_from_dict(param_name, data)
        elif type(data) is list:
            integer_parameter = IntegerParam._create_from_list_legacy(param_name, data)
        else:
            integer_parameter = None
        return integer_parameter

    @staticmethod
    def _create_from_dict(param_name, data : dict):
        is_mutable(param_name, str(data['flags']))
        integer_parameter = IntegerParam(
            name=f"\"{param_name}\"",
            flags="",
            enum_name=data['enum'],
            default=data['default'],
            min=data['min'],
            max=data['max'],
        )
        if 'note' in data:
            integer_parameter.note = data['note']
        else:
            integer_parameter.note = ""
        return integer_parameter

    @staticmethod
    def _create_from_list_legacy(param_name, data : list):
        """Input example: param_name : ["Integer", "ENUM_NAME", "mutable", -1, -1, 31]"""
        is_mutable(param_name, str(data[2]))
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
    def create(param_name, data):
        if type(data) is dict:
            string_param = StringParam._create_from_dict(param_name, data)
        elif type(data) is list:
            string_param = StringParam._create_from_list_legacy(param_name, data)
        else:
            string_param = None
        return string_param

    @staticmethod
    def _create_from_dict(param_name, data : dict):
        string_param = StringParam(
            name=f"\"{param_name}\"",
            default="\"{}\"".format(data['default']),
            mutability=is_mutable(param_name, str(data['flags']))
        )
        if 'note' in data:
            string_param.note = data['note']
        else:
            string_param.note = ""
        return string_param

    @staticmethod
    def _create_from_list_legacy(param_name, data : list):
        """Input example: param_name : ["data_type",  "ENUM_NAME"]"""
        string_param = StringParam(
            name=f"\"{param_name}\"",
            default="\"{}\"".format(data[3]),
            mutability=is_mutable(param_name, str(data[2]))
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
