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

logger = logging.getLogger(__name__)


def main(directories: list):
    assert isinstance(directories, list)

    for dir in directories:
        if not os.path.exists(dir):
            sys.exit(f"The directory {dir} is not exist. Abort.")

    logging.debug(f"Directories: {directories}")

if __name__=="__main__":
    logging.basicConfig(level=logging.DEBUG)
    logger.setLevel(logging.DEBUG)

    from argparse import ArgumentParser
    parser = ArgumentParser(description=__doc__)
    parser.add_argument('directories', nargs='+', help='Directories to search for source files')

    args = parser.parse_args()
    main(args.directories)
