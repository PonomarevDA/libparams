cmake_minimum_required(VERSION 3.10)
project(tests VERSION 0.1)
set(CMAKE_CXX_STANDARD 17)

cmake_path(GET CMAKE_CURRENT_LIST_DIR PARENT_PATH TESTS_DIR)
cmake_path(GET TESTS_DIR PARENT_PATH LIBPARAMS_ROOT_DIR)
set(LIBPARAMS_SOURCE_CODE_PATH ${LIBPARAMS_ROOT_DIR}/libparams)
set(PLATFORM_SPECIFIC_PATH ${LIBPARAMS_ROOT_DIR}/platform_specific/ubuntu)
set(PARAMS_PATH ${LIBPARAMS_ROOT_DIR}/build/tests/params_generator)

include_directories(
    ${LIBPARAMS_SOURCE_CODE_PATH}
    ${PLATFORM_SPECIFIC_PATH}
    ${PARAMS_PATH}
)

enable_testing()
find_package(GTest REQUIRED)
include(GoogleTest)

FILE(GLOB GENERATED_PARAMS ${PARAMS_PATH}/*.c*)

add_executable(${PROJECT_NAME} test.cpp
               ${LIBPARAMS_SOURCE_CODE_PATH}/storage.c
               ${LIBPARAMS_SOURCE_CODE_PATH}/rom.c
               ${GENERATED_PARAMS}
               ${PLATFORM_SPECIFIC_PATH}/flash_driver.cpp)
target_link_libraries(${PROJECT_NAME} GTest::GTest GTest::Main)
gtest_discover_tests(${PROJECT_NAME})
