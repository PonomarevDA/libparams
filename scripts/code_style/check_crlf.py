#!/usr/bin/env python3
#
# Copyright (c) 2024 Dmitry Ponomarev <ponomarevda96@gmail.com>
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.
"""
Verify that all files has CR in the current directory
"""
import os
import sys

def is_crlf(file_path: str) -> bool:
    assert isinstance(file_path, str)

    with open(file_path, 'rb') as file:
        for line in file:
            if b'\r\n' in line:
                return True
    return False

def check_files_in_directory(directory: str, exclude_directories=('.git', 'build')) -> list:
    assert isinstance(directory, str)
    assert isinstance(exclude_directories, tuple)

    crlf_files = []
    for root, dirs, files in os.walk(directory):
        dirs[:] = [d for d in dirs if d not in exclude_directories]
        for file in files:
            file_path = os.path.join(root, file)
            if is_crlf(file_path):
                crlf_files.append(file_path)
    return crlf_files

def main():
    current_directory = os.getcwd()
    crlf_files = check_files_in_directory(current_directory)

    if crlf_files:
        for file_path in crlf_files:
            print(file_path)
        sys.exit(1)
    else:
        sys.exit(0)

if __name__ == "__main__":
    main()
