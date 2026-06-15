function(add_libparams_stm32_platform_test target platform device_define)
    cmake_path(GET CMAKE_CURRENT_SOURCE_DIR PARENT_PATH PLATFORM_SPECIFIC_TESTS_DIR)
    cmake_path(GET PLATFORM_SPECIFIC_TESTS_DIR PARENT_PATH TESTS_DIR)
    cmake_path(GET TESTS_DIR PARENT_PATH ROOT_DIR)

    set(LIBPARAMS_PLATFORM ${platform})
    include(${ROOT_DIR}/libparams.cmake)

    add_executable(${target}
        ${libparamsSrc}
        ${TESTS_DIR}/params/params.c
        ${CMAKE_CURRENT_SOURCE_DIR}/main.c
        ${CMAKE_CURRENT_SOURCE_DIR}/application.c
    )

    target_include_directories(${target} PRIVATE
        ${libparamsHeaders}
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${TESTS_DIR}
        ${TESTS_DIR}/params
    )

    target_compile_definitions(${target} PRIVATE
        USE_HAL_DRIVER
        ${device_define}
    )

    target_compile_options(${target} PRIVATE
        -mcpu=cortex-m3
        -mthumb
        -Og
        -Wall
        -Wpedantic
        -Werror
        -fdata-sections
        -ffunction-sections
        -Wcast-align
        -g
        -gdwarf-2
    )

    target_link_options(${target} PRIVATE
        -mcpu=cortex-m3
        -mthumb
        -specs=nano.specs
        -specs=nosys.specs
        -Wall
        -Wpedantic
        -Werror
        -Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/${target}.map,--cref
        -Wl,--gc-sections
    )

    target_link_libraries(${target} PRIVATE c m nosys)

    add_custom_command(TARGET ${target} POST_BUILD
        COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${target}>
        COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${target}> ${target}.hex
        COMMAND ${CMAKE_OBJCOPY} -O binary -S $<TARGET_FILE:${target}> ${target}.bin
        BYPRODUCTS ${target}.hex ${target}.bin
    )
endfunction()
