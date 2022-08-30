#!/usr/bin/env python3
import os
import sys
from glob import glob

LANGUAGE_C = 0
LANGUAGE_CPP = 1


class AbstractGenerator:
    def __init__(self) -> None:
        self.input_param_file_name = ""
        self.generated_source_file_name = ""
        self.generated_header_file_name = ""
        self.generated_hpp_file_name = "params.hpp"
        self.start_source_line = "IntegerDesc_t integer_desc_pool[] __attribute__((unused)) = {"
        self.end_header_line = ""
        self.include_storage_line = "#include \"storage.h\"\n\n"
        self.end_source_line = "};"
        self.array_line = "IntegerParamValue_t integer_values_pool[sizeof(integer_desc_pool) / sizeof(IntegerDesc_t)];\n"
        self.start_header_line = ""

class CGenerator(AbstractGenerator):
    def __init__(self) -> None:
        super().__init__()
        self.input_param_file_name = "*_params.c"
        self.generated_source_file_name = "params.c"
        self.generated_header_file_name = "params.h"
        self.end_header_line = "} IntParamsIndexes;"
        self.include_storage_line = "extern \"C\" {\n    #include \"storage.h\"\n}\n\n"
        self.start_header_line = "typedef enum {"

class CppGenerator(AbstractGenerator):
    def __init__(self) -> None:
        super().__init__()
        self.input_param_file_name = "*_params.cpp"
        self.generated_source_file_name = "params.cpp"
        self.generated_header_file_name = "params.hpp"
        self.end_header_line = "};"
        self.include_storage_line = ""
        self.start_header_line = "enum class IntParamsIndexes {"

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

def generate_source_files(input_files, output_file, generator):
    out_fd = open(output_file + generator.generated_source_file_name, 'w')
    out_fd.write(f"#include \"{generator.generated_header_file_name}\"\n")
    out_fd.write(generator.include_storage_line)

    out_fd.write(f"{generator.start_source_line}\n")
    generate(input_files, out_fd, generator.start_source_line, generator.end_source_line)
    out_fd.write(f"{generator.end_source_line}\n")
    out_fd.write(generator.array_line)
    out_fd.close()

def generate_header_files(input_files, output_file, generator):
    out_fd = open(output_file+generator.generated_header_file_name, 'w')
    out_fd.write("#pragma once\n\n")
    out_fd.write(f"{generator.start_header_line}\n")
    generate(input_files, out_fd, generator.start_header_line, generator.end_header_line)
    out_fd.write("    INTEGER_PARAMS_AMOUNT\n")
    out_fd.write(f"{generator.end_header_line}\n")
    out_fd.close()

def sort_files_by_priorities(files):
    """ It is preferred to keep Cyphal parameters first. """
    for file in files:
        if file.endswith('cyphal_params.cpp'):
            files.insert(0, files.pop(files.index(file)))
            break
    return files

def find_src_files(input_file, generator):
    config_fd = open(input_file, 'r')
    source_files_paths = config_fd.readlines()
    all_src_files = []
    for line in source_files_paths:
        source_files_path = line.rstrip()
        src_files = [y for x in os.walk(source_files_path) for y in glob(os.path.join(x[0], generator.input_param_file_name))]
        if len(src_files) != 0:
            all_src_files.append(*src_files)
    config_fd.close()

    all_src_files = sort_files_by_priorities(all_src_files)
    return all_src_files

if __name__=="__main__":
    input_dir, output_dir, language = parse_args()

    if language == LANGUAGE_C:
        generator = CGenerator()
    elif language == LANGUAGE_CPP:
        generator = CppGenerator()
    else:
        exit()

    src_files = find_src_files(input_dir, generator)
    generate_source_files(src_files, output_dir, generator)
    generate_header_files(src_files, output_dir, generator)
