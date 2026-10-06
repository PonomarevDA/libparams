#!/usr/bin/env python3
#
# Copyright (c) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
#                    Anastasiia Stepanova <asiiapine@gmail.com>
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.

"""Integer and string parameters described in YAML files."""

import logging
from dataclasses import dataclass
from typing import List, Tuple, Union
import yaml

logger = logging.getLogger("libparams-gen")

REQUIRED_KEYS = {
    "integer": ("enum", "flags", "default", "min", "max"),
    "string": ("enum", "flags", "default"),
    "port": ("data_type", "enum_base"),
}


class ParamsError(Exception):
    """A YAML file doesn't describe valid parameters."""


@dataclass
class BaseParam:
    name: str = ""
    default: Union[int, str] = 0
    note: str = ""
    mutability: str = "MUTABLE"
    enum_name: str = ""


@dataclass
class IntegerParam(BaseParam):
    min: int = 0
    max: int = 0
    is_required: str = "false"

    @staticmethod
    def create(param_name: str, data: dict) -> "IntegerParam":
        return IntegerParam(
            name=param_name,
            enum_name=data['enum'],
            default=data['default'],
            min=data['min'],
            max=data['max'],
            mutability=is_mutable(param_name, str(data['flags'])),
            note=data.get('note', ""),
            is_required="true" if data.get('is_required') else "false",
        )

    @staticmethod
    def create_port_id(param_name: str, enum_base: str) -> "IntegerParam":
        return IntegerParam(
            name=f"{param_name}.id",
            enum_name=f"{enum_base}_ID",
            default=65535,
            min=0,
            max=65535,
        )


@dataclass
class StringParam(BaseParam):
    @staticmethod
    def create(param_name: str, data: dict) -> "StringParam":
        return StringParam(
            name=param_name,
            default=data['default'],
            enum_name=data['enum'],
            mutability=is_mutable(param_name, str(data['flags'])),
            note=data.get('note', ""),
        )

    @staticmethod
    def create_port_type(param_name: str, data_type: str, enum_base: str) -> "StringParam":
        return StringParam(
            name=f"{param_name}.type",
            default=data_type,
            enum_name=f"{enum_base}_TYPE",
            mutability="IMMUTABLE",
        )


def is_mutable(param_name: str, mutability_str: str) -> str:
    if mutability_str in ["True", "immutable"]:
        mutability = "IMMUTABLE"
    elif mutability_str in ["False", "mutable"]:
        mutability = "MUTABLE"
    else:
        raise ParamsError(f"{param_name}: flags must be `mutable` or `immutable`, "
                          f"got `{mutability_str}`")
    if mutability_str in ["True", "False"]:
        logger.warning("%s: flags `%s` is deprecated, use `mutable` or `immutable`",
                       param_name, mutability_str)
    return mutability


def load_params(yaml_files: List[str]) -> Tuple[List[IntegerParam], List[StringParam]]:
    """Read the parameters in file order. A port adds an integer id and a string type."""
    integers = []
    strings = []
    for yaml_file in yaml_files:
        with open(yaml_file, "r", encoding="utf-8") as yaml_fd:
            params = yaml.safe_load(yaml_fd) or {}
        if not isinstance(params, dict):
            raise ParamsError(f"{yaml_file}: expected a mapping of parameter names")

        for name, data in params.items():
            if not isinstance(data, dict):
                raise ParamsError(f"{yaml_file}: {name}: expected a mapping "
                                  "(the legacy list format is not supported)")
            param_type = str(data.get('type', "")).lower()
            if param_type not in REQUIRED_KEYS:
                raise ParamsError(f"{yaml_file}: {name}: unknown type `{data.get('type')}`, "
                                  "expected Integer, String or Port")
            missing = [key for key in REQUIRED_KEYS[param_type] if key not in data]
            if missing:
                raise ParamsError(f"{yaml_file}: {name}: missing {', '.join(missing)}")

            if param_type == "port":
                integers.append(IntegerParam.create_port_id(name, data['enum_base']))
                strings.append(StringParam.create_port_type(name, data['data_type'],
                                                            data['enum_base']))
            elif param_type == "integer":
                integers.append(IntegerParam.create(name, data))
            else:
                strings.append(StringParam.create(name, data))
    return integers, strings
