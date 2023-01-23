cmake_minimum_required(VERSION 3.10)
project(tests VERSION 0.1)
set(CMAKE_CXX_STANDARD 17)

set(LIBPARAMS_PATH ../../libparams)
set(PLATFORM_SPECIFIC_PATH ../../platform_specific/ubuntu)
set(PARAMS_PATH ../../build/tests/params_generator_c)

include_directories(
    ${LIBPARAMS_PATH}
    ${PLATFORM_SPECIFIC_PATH}
    ${PARAMS_PATH}
)

enable_testing()
find_package(GTest REQUIRED)
include(GoogleTest)

add_executable(${PROJECT_NAME} test.cpp
                               ${LIBPARAMS_PATH}/storage.c
                               ${LIBPARAMS_PATH}/rom.c
                               ${PARAMS_PATH}/params.c
                               ${PARAMS_PATH}/string_params.c
                               ${PLATFORM_SPECIFIC_PATH}/flash_driver.cpp)
target_link_libraries(${PROJECT_NAME} GTest::GTest GTest::Main)
gtest_discover_tests(${PROJECT_NAME})
