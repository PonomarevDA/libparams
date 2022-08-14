#!/usr/bin/env python3
import os
import sys
from glob import glob

GENERATED_C_FILE_NAME = "params.cpp"
GENERATED_H_FILE_NAME = "params.hpp"

def print_help():
    usage_line = "python3 params_generate_array.py [input_dir] [output_dir]"
    short_desc_line = "Generate a single C++ array and enum class with parameters based on several inputs."
    print(f"{usage_line}\n    {short_desc_line}")


def parse_args():
    num_of_args = len(sys.argv)
    if num_of_args != 3:
        print_help()
        exit()

    input_dir = sys.argv[1]
    if not os.path.exists(input_dir):
        print(f"Input directotry `{input_dir}` is not exist!")
        exit()

    output_dir = sys.argv[2]
    if not os.path.exists(output_dir):
        print(f"Input directotry `{output_dir}` is not exist!")
        exit()

    return input_dir, output_dir

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

def generate_c_files(input_files, output_file):
    START_LINE = "IntegerDesc_t integer_desc_pool[] = {"
    END_LINE = "};"
    out_fd = open(output_file + GENERATED_C_FILE_NAME, 'w')
    out_fd.write(f"#include \"{GENERATED_H_FILE_NAME}\"\n")
    out_fd.write("extern \"C\" {\n    #include \"storage.h\"\n}\n")

    out_fd.write(f"{START_LINE}\n")
    generate(input_files, out_fd, START_LINE, END_LINE)
    out_fd.write(f"{END_LINE}\n")
    out_fd.write("IntegerParamValue_t integer_values_pool[sizeof(integer_desc_pool) / sizeof(IntegerDesc_t)];\n")
    out_fd.close()

def generate_h_files(input_files, output_file):
    START_LINE = "enum class IntParamsIndexes {"
    END_LINE = "};"
    out_fd = open(output_file+GENERATED_H_FILE_NAME, 'w')
    out_fd.write("#pragma once\n")
    out_fd.write(f"{START_LINE}\n")
    generate(input_files, out_fd, START_LINE, END_LINE)
    out_fd.write("    INTEGER_AMOUNT\n")
    out_fd.write(f"{END_LINE}\n")
    out_fd.close()

def sort_files_by_priorities(files):
    """ It is preferred to keep Cyphal parameters first. """
    for file in files:
        if file.endswith('cyphal_params.cpp'):
            files.insert(0, files.pop(files.index(file)))
            break
    return files

if __name__=="__main__":
    input_dir, output_dir = parse_args()
    src_files = [y for x in os.walk(input_dir) for y in glob(os.path.join(x[0], '*_params.cpp'))]
    src_files = sort_files_by_priorities(src_files)

    generate_c_files(src_files, output_dir)
    generate_h_files(src_files, output_dir)
