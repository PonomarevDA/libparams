#!/usr/bin/env python3
#
# Copyright (c) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.
"""
Check code style with astyle
"""
import os
import sys
import logging
import subprocess
from typing import Optional
from pathlib import Path

logger = logging.getLogger(__name__)

def get_source_files(directories: list, extensions=('.c', '.cpp', '.h', '.hpp')) -> list:
    assert isinstance(directories, list)
    assert isinstance(extensions, tuple)

    source_files = []
    for directory in directories:
        for path in Path(directory).rglob('*'):
            if path.suffix in extensions and path.is_file():
                source_files.append(path)
    return source_files

def astyle_single_file(origin_file_path: str,
                       pretty_file_path: str,
                       astylerc_path: Optional[str]=None) -> None:
    astyle_check_cmd = ['astyle']
    if astylerc_path is not None:
        astyle_check_cmd.append(f"--options={astylerc_path}")

    with open(origin_file_path, 'r', encoding='UTF-8') as infile:
        logger.debug(" ".join(astyle_check_cmd))
        result = subprocess.run(
            astyle_check_cmd,
            input=infile.read(),
            capture_output=True,
            text=True,
            check=True,
        )

    with open(pretty_file_path, 'w', encoding='UTF-8') as outfile:
        outfile.write(result.stdout)

def compare_two_files_wth_git_diff(first_file_path: str, second_file_path: str) -> str:
    git_diff_cmd = [
        'git',
        '--no-pager',
        'diff',
        '--no-index',
        '--minimal',
        '--histogram',
        '--color=always',
        first_file_path,
        second_file_path
    ]

    logger.debug(" ".join(git_diff_cmd))
    result = subprocess.run(git_diff_cmd, capture_output=True, text=True, check=False)
    return result.stdout


def check_single_file(source_file_path: str, astylerc_path: Optional[str]=None) -> str:
    assert isinstance(source_file_path, str)
    assert isinstance(astylerc_path, str) or astylerc_path is None

    temp_pretty_file = f"{source_file_path}.pretty"
    astyle_single_file(source_file_path, temp_pretty_file, astylerc_path)
    res = compare_two_files_wth_git_diff(str(source_file_path), temp_pretty_file)
    os.remove(temp_pretty_file)

    return res

def main(directories: list, astylerc_path: Optional[str]):
    assert isinstance(directories, list)
    assert isinstance(astylerc_path, str) or astylerc_path is None

    for directory in directories:
        if not os.path.exists(directory):
            sys.exit(f"Error: the directory `{directory}` is not exist. Abort.")

    if astylerc_path is not None and not os.path.exists(astylerc_path):
        sys.exit(f"Error: astylerc path `{astylerc_path}` specified, but can't be found. Abort.")

    logging.debug("Directories: %s", directories)

    files = get_source_files(directories)
    res = ""
    for file in files:
        logging.debug("\n\n%s", file)
        res += check_single_file(source_file_path=str(file), astylerc_path=astylerc_path)

    if len(res) > 0:
        print(res)
        sys.exit("ERROR: Fix code style!")

if __name__=="__main__":
    logging.basicConfig(level=logging.INFO)
    logger.setLevel(logging.INFO)

    from argparse import ArgumentParser
    parser = ArgumentParser(description=__doc__)
    parser.add_argument('directories', nargs='+',
                        help='Directories to search for source files')
    parser.add_argument("--astylerc", type=str, required=False,
                        help="Optional path to astylerc file")

    args = parser.parse_args()
    main(args.directories, args.astylerc)
