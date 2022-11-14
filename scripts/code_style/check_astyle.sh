#!/bin/bash
set -e
THIS_SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
SCRIPTS_DIR="$(dirname "$THIS_SCRIPT_DIR")"
REPO_DIR="$(dirname "$SCRIPTS_DIR")"
CODE_STYLE_DIR=$REPO_DIR/code_style

SOURCE_FILE_PATHES=$($THIS_SCRIPT_DIR/get_pathes.sh) $CODE_STYLE_DIR/astyle_check_all.sh