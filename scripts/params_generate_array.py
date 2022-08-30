#!/usr/bin/env python3
import os
import sys
from glob import glob

LANGUAGE_C = 0
LANGUAGE_CPP = 1

INPUT_PARAM_FILE_NAME = ["*_params.c", "*_params.cpp"]
GENERATED_SOURCE_FILE_NAME = ["params.c", "params.cpp"]
GENERATED_HEADER_FILE_NAME = ["params.h", "params.hpp"]
GENERATED_HPP_FILE_NAME = "params.hpp"
START_SOURCE_LINE = "IntegerDesc_t integer_desc_pool[] __attribute__((unused)) = {"
END_HEADER_LINE = ["} IntParamsIndexes;", "};"]
INCLUDE_STORAGE_LINE = ["#include \"storage.h\"\n\n", "extern \"C\" {\n    #include \"storage.h\"\n}\n\n"]

END_SOURCE_LINE = "};"
START_HEADER_LINE = ["typedef enum {", "enum class IntParamsIndexes {"]


def print_help():
    usage_line = "python3 params_generate_array.py [input_dir] [output_dir] [language: `c++` or `c`]"
    short_desc_line = "Generate a single C++ array and enum class with parameters based on several inputs."
    example_desc_line = "Example: python3 params_generate_array.py ./Src/ ./ c++"
    print(f"{usage_line}\n    {short_desc_line}\n    {example_desc_line}")


def parse_args():
    num_of_args = len(sys.argv)
    if num_of_args != 4:
        print_help()
        exit()

    input_dir = sys.argv[1]
    if not os.path.exists(input_dir):
        print(f"Input file with paths `{input_dir}` is not exist!")
        exit()

    output_dir = sys.argv[2]
    if not os.path.exists(output_dir):
        print(f"Output directotry `{output_dir}` is not exist!")
        exit()

    language = sys.argv[3]
    if language == "c":
        language = LANGUAGE_C
    elif language == "c++":
        language = LANGUAGE_CPP
    else:
        print(f"Language `{language}` is not supported!")
        exit()

    print("Config of the parameters generator:")
    print(f"- Input file with paths: `{input_dir}`")
    print(f"- Output directotry: `{output_dir}`")
    print(f"- Language: `{sys.argv[3]}`")

    return input_dir, output_dir, language

def generate(input_files, out_fd, start_line, end_line):
    for src_file in input_files:
        count = 0
        first_param_idx = None
        last_param_idx = None
        fd = open(src_file, 'r')
        lines = fd.readlines()
        for line in lines:
            if line.find(start_line) != -1:
                first_param_idx = count + 1
            if first_param_idx is not None and line.find(end_line) != -1:
                last_param_idx = count - 1
                break
            count += 1
        fd.close()

        if last_param_idx is not None and last_param_idx >= first_param_idx:
            print(src_file, last_param_idx - first_param_idx + 1)
            for line_idx in range(first_param_idx, last_param_idx + 1):
                out_fd.write(lines[line_idx])

def generate_source_files(input_files, output_file, language):
    out_fd = open(output_file + GENERATED_SOURCE_FILE_NAME[language], 'w')
    out_fd.write(f"#include \"{GENERATED_HEADER_FILE_NAME[language]}\"\n")
    out_fd.write(INCLUDE_STORAGE_LINE[language])

    out_fd.write(f"{START_SOURCE_LINE}\n")
    generate(input_files, out_fd, START_SOURCE_LINE, END_SOURCE_LINE)
    out_fd.write(f"{END_SOURCE_LINE}\n")
    out_fd.write("IntegerParamValue_t integer_values_pool[sizeof(integer_desc_pool) / sizeof(IntegerDesc_t)];\n")
    out_fd.close()

def generate_header_files(input_files, output_file, language):
    out_fd = open(output_file+GENERATED_HEADER_FILE_NAME[language], 'w')
    out_fd.write("#pragma once\n\n")
    out_fd.write(f"{START_HEADER_LINE[language]}\n")
    generate(input_files, out_fd, START_HEADER_LINE[language], END_HEADER_LINE[language])
    out_fd.write("    INTEGER_PARAMS_AMOUNT\n")
    out_fd.write(f"{END_HEADER_LINE[language]}\n")
    out_fd.close()

def sort_files_by_priorities(files):
    """ It is preferred to keep Cyphal parameters first. """
    for file in files:
        if file.endswith('cyphal_params.cpp'):
            files.insert(0, files.pop(files.index(file)))
            break
    return files

def find_src_files(input_file, language):
    config_fd = open(input_file, 'r')
    source_files_paths = config_fd.readlines()
    all_src_files = []
    for line in source_files_paths:
        source_files_path = line.rstrip()
        src_files = [y for x in os.walk(source_files_path) for y in glob(os.path.join(x[0], INPUT_PARAM_FILE_NAME[language]))]
        if len(src_files) != 0:
            all_src_files.append(*src_files)
    config_fd.close()

    all_src_files = sort_files_by_priorities(all_src_files)
    return all_src_files

if __name__=="__main__":
    input_dir, output_dir, language = parse_args()
    src_files = find_src_files(input_dir, language)

    generate_source_files(src_files, output_dir, language)
    generate_header_files(src_files, output_dir, language)
