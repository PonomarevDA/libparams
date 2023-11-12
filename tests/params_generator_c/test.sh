#!/bin/bash
THIS_SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
TESTS_DIR="$(dirname "${THIS_SCRIPT_DIR}")"
REPO_DIR="$(dirname "${TESTS_DIR}")"
SCRIPTS_DIR="${REPO_DIR}/scripts"
BUILD_DIR="${REPO_DIR}/build/tests/params_generator_c"

mkdir -p ${BUILD_DIR}

${SCRIPTS_DIR}/params_generate_array_from_yaml.py \
    ${BUILD_DIR} \
    c \
    params \
    ${THIS_SCRIPT_DIR}/baro.yaml ${THIS_SCRIPT_DIR}/mag.yaml

cd $THIS_SCRIPT_DIR
cmake -S . -B ${BUILD_DIR}
cmake --build ${BUILD_DIR}
cd ${BUILD_DIR}
ctest --verbose
