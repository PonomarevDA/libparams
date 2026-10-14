# Copyright (c) 2023-2026 Dmitry Ponomarev <ponomarevda96@gmail.com>
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.

include_guard(GLOBAL)

# Generate the parameter tables from YAML files at configure time:
#
# libparams_generate_params(
#     OUT_DIR <dir>                 # <OUT_FILE_NAME>.cpp/.hpp, or .c/.h for LANGUAGE c
#     YAML <file>...                # parameters in index order
#     [LANGUAGE c++|c]              # default: c++
#     [OUT_FILE_NAME <name>]        # default: params
#     [DEFAULT_PARAMS_DIR <dir>]    # SITL: also write <DEFAULT_PARAMS_NAME>_<page>.yaml
#     [DEFAULT_PARAMS_NAME <name>]  # default: default_params
#     [DOCS <file>]                 # markdown table of the parameters
# )
#
# Relative YAML and DOCS paths are relative to the current source dir, relative output
# dirs to the current binary dir. CMake re-configures when a YAML file changes.
function(libparams_generate_params)
  cmake_parse_arguments(ARG ""
    "OUT_DIR;LANGUAGE;OUT_FILE_NAME;DEFAULT_PARAMS_DIR;DEFAULT_PARAMS_NAME;DOCS"
    "YAML" ${ARGN})
  if(NOT ARG_OUT_DIR OR NOT ARG_YAML)
    message(FATAL_ERROR "libparams_generate_params: OUT_DIR and YAML are required.")
  endif()
  find_package(Python3 REQUIRED COMPONENTS Interpreter)
  cmake_path(GET CMAKE_CURRENT_FUNCTION_LIST_DIR PARENT_PATH root_dir)
  set(scripts_dir ${root_dir}/scripts)

  set(yaml_files)
  foreach(yaml_file ${ARG_YAML})
    get_filename_component(yaml_file ${yaml_file} ABSOLUTE)
    list(APPEND yaml_files ${yaml_file})
  endforeach()
  set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${yaml_files})

  get_filename_component(out_dir ${ARG_OUT_DIR} ABSOLUTE BASE_DIR ${CMAKE_CURRENT_BINARY_DIR})
  set(args --out-dir ${out_dir} -f ${yaml_files})
  if(ARG_LANGUAGE)
    list(APPEND args --language ${ARG_LANGUAGE})
  endif()
  if(ARG_OUT_FILE_NAME)
    list(APPEND args --out-file-name ${ARG_OUT_FILE_NAME})
  endif()
  if(ARG_DEFAULT_PARAMS_DIR)
    get_filename_component(default_params_dir ${ARG_DEFAULT_PARAMS_DIR} ABSOLUTE
                           BASE_DIR ${CMAKE_CURRENT_BINARY_DIR})
    list(APPEND args --default-params-dir ${default_params_dir})
  endif()
  if(ARG_DEFAULT_PARAMS_NAME)
    list(APPEND args --default-params-name ${ARG_DEFAULT_PARAMS_NAME})
  endif()
  execute_process(
    COMMAND ${Python3_EXECUTABLE} ${scripts_dir}/generate_params.py ${args}
    COMMAND_ERROR_IS_FATAL ANY
  )

  if(ARG_DOCS)
    get_filename_component(docs ${ARG_DOCS} ABSOLUTE)
    execute_process(
      COMMAND ${Python3_EXECUTABLE} ${scripts_dir}/generate_docs.py --output ${docs} ${yaml_files}
      COMMAND_ERROR_IS_FATAL ANY
    )
  endif()
endfunction()
