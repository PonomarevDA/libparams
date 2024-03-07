#!/usr/bin/env python3
#
# Copyright (c) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.

"""Integer and String parameters."""

import sys
from dataclasses import dataclass
from color_logging import log_warn, log_err

@dataclass
class BaseParam:
    name: str = ""
    default: int = 0
    note: str = ""
    mutability: str = "MUTABLE"

@dataclass
class IntegerParam(BaseParam):
    """
    Suitable for:
    - Cyphal    uavcan.register.Value.1.0.integer32
    - DroneCAN  uavcan.param.Value.integer_value
    """
    enum_name: str = ""
    flags: str = ""
    min: int = 0
    max: int = 0
    is_required: str = "false"

    @staticmethod
    def create(param_name, data):
        if isinstance(data, dict):
            integer_parameter = IntegerParam._create_from_dict(param_name, data)
        elif isinstance(data, list):
            integer_parameter = IntegerParam._create_from_list_legacy(param_name, data)
        else:
            integer_parameter = None

        if not hasattr(integer_parameter, "note"):
            integer_parameter.note = ""

        return integer_parameter

    @staticmethod
    def create_port_id(param_name, enum_base : str):
        id_register = IntegerParam(
            name=f"\"{param_name}.id\"",
            flags="",
            enum_name=f"{enum_base}_ID",
            default=65535,
            min=0,
            max=65535,
        )
        return id_register

    @staticmethod
    def _create_from_dict(param_name, data : dict):
        integer_parameter = IntegerParam(
            name=f"\"{param_name}\"",
            flags="",
            enum_name=data['enum'],
            default=data['default'],
            min=data['min'],
            max=data['max'],
            mutability=is_mutable(param_name, str(data['flags']))
        )

        integer_parameter.note = data['note'] if 'note' in data else ""

        if 'is_required' in data and data['is_required']:
            integer_parameter.is_required = "true"
        else:
            integer_parameter.is_required = "false"

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

@dataclass
class StringParam(BaseParam):
    """
    Suitable for:
    - Cyphal    uavcan.register.Value.1.0.string
    - DroneCAN  uavcan.param.Value.string_value
    """
    @staticmethod
    def create(param_name, data):
        if isinstance(data, dict):
            string_param = StringParam._create_from_dict(param_name, data)
        elif isinstance(data, list):
            string_param = StringParam._create_from_list_legacy(param_name, data)
        else:
            string_param = None

        if not hasattr(string_param, "note"):
            string_param.note = ""

        return string_param

    @staticmethod
    def create_port_type(param_name, data_type : str):
        type_register = StringParam(
            name=f"\"{param_name}.type\"",
            default=f"\"{data_type}\"",
            mutability="IMMUTABLE"
        )
        return type_register

    @staticmethod
    def _create_from_dict(param_name, data : dict):
        string_param = StringParam(
            name=f"\"{param_name}\"",
            default=f"\"{data['default']}\"",
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
            default=f"\"{data[3]}\"",
            mutability=is_mutable(param_name, str(data[2]))
        )
        return string_param

def is_mutable(param_name : str, mutability_str : str):
    assert isinstance(param_name, str)
    assert isinstance(mutability_str, str)
    name = f"\"{param_name}\""
    if mutability_str in ["True", "immutable"]:
        mutability = "IMMUTABLE"
    elif mutability_str in ["False", "mutable"]:
        mutability = "MUTABLE"
    else:
        log_err((f"Mutability of {name} can't be determined: "
                 f"{mutability_str} ({type(mutability_str)})"))
        sys.exit()
    if mutability_str in ["True", "False"]:
        log_warn((f"Mutability of {name} will be deprecated soon: "
                  f"{mutability_str}. Use `mutable` or `immutable`."))

    return mutability
