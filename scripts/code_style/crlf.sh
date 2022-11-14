#!/bin/bash
set -e
THIS_SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
SCRIPTS_DIR="$(dirname "$THIS_SCRIPT_DIR")"
REPO_DIR="$(dirname "$SCRIPTS_DIR")"
CODE_STYLE_DIR=$REPO_DIR/code_style

SOURCE_FILE_PATHES=()
for rel_path in $(find $dir -type f); do
    abs_path=$(readlink -f $rel_path)
    if [[ $abs_path == */build/* ]] || \
       [[ $abs_path == */doc/* ]] || \
       [[ $abs_path == */.git/* ]] || \
       [[ $abs_path == */code_style/* ]] || \
       [[ $abs_path == */Release.launch ]]; then
        continue
    fi
    SOURCE_FILE_PATHES+=($abs_path)
done

source $CODE_STYLE_DIR/crlf.sh
if [ "$?" -eq "-1" ]; then
    exit -1;
fi
