#!/bin/bash
set -e
THIS_SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
pathes=$($THIS_SCRIPT_DIR/get_pathes.sh)
cpplint $pathes