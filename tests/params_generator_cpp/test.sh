THIS_SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
TESTS_DIR="$(dirname "${THIS_SCRIPT_DIR}")"
REPO_DIR="$(dirname "${TESTS_DIR}")"
SCRIPTS_DIR="${REPO_DIR}/scripts"
BUILD_DIR="${REPO_DIR}/build/tests/params_generator_cpp"

mkdir -p ${BUILD_DIR}

${SCRIPTS_DIR}/generate_params.py --out-dir ${BUILD_DIR} -f ${TESTS_DIR}/baro.yaml ${TESTS_DIR}/mag.yaml

cmake -S . -B ${BUILD_DIR}
cmake --build ${BUILD_DIR}
cd ${BUILD_DIR}
ctest --verbose
