#!/usr/bin/env python3
import os
import sys
from glob import glob
from color_logging import log_info, log_warn, log_err

LANGUAGE_C = 0
LANGUAGE_CPP = 1


class ParamsGenerator:
    def __init__(self) -> None:
        self.input_param_file_name = ""
        self.generated_source_file_name = ""
        self.generated_header_file_name = ""
        self.in_start_src_line = "IntegerDesc_t integer_desc_pool[] __attribute__((unused)) = {"
        self.out_start_src_line = "IntegerDesc_t integer_desc_pool[] = {"
        self.start_hdr_line = ""
        self.end_hdr_line = ""
        self.include_storage_line = ""
        self.end_src_line = "};"
        self.array_line = "IntegerParamValue_t integer_values_pool[sizeof(integer_desc_pool) / sizeof(IntegerDesc_t)];\n"

    def prepare_head_of_src_file(self, output_file):
        out_src_fd = open(output_file + generator.generated_source_file_name, 'w')
        out_src_fd.write(f"#include \"{self.generated_header_file_name}\"\n")
        out_src_fd.write(self.include_storage_line)
        out_src_fd.write(f"{self.out_start_src_line}\n")
        return out_src_fd

    def finish_src_file(self, out_src_fd):
        out_src_fd.write(f"{self.end_src_line}\n")
        out_src_fd.write(self.array_line)
        out_src_fd.close()

    def write_beginning_header_part(self, output_file):
        out_header_fd = open(output_file+generator.generated_header_file_name, 'w')
        out_header_fd.write("#pragma once\n")
        out_header_fd.write("\n")
        out_header_fd.write(f"{self.start_hdr_line}\n")
        return out_header_fd

    def finish_hdr_file(self, out_header_fd):
        out_header_fd.write("    INTEGER_PARAMS_AMOUNT\n")
        out_header_fd.write(f"{self.end_hdr_line}\n")
        out_header_fd.close()

    @staticmethod
    def process_single_file(src_file, out_fd, start_line, end_line):
        num_of_params = 0
        count = 0
        first_param_idx = None
        last_param_idx = None
        in_fd = open(src_file, 'r')
        lines = in_fd.readlines()
        for line in lines:
            if line.find(start_line) != -1:
                first_param_idx = count + 1
            if first_param_idx is not None and line.find(end_line) != -1:
                last_param_idx = count - 1
                break
            count += 1
        in_fd.close()

        if last_param_idx is not None and last_param_idx >= first_param_idx:
            num_of_params = last_param_idx - first_param_idx + 1
            for line_idx in range(first_param_idx, last_param_idx + 1):
                out_fd.write(lines[line_idx])
            out_fd.write("\n")

        return num_of_params

    def generate_everything(self, input_files, output_file):
        out_src_fd = self.prepare_head_of_src_file(output_file)
        out_hdr_fd = self.write_beginning_header_part(output_file)
        log_info(f"Number of files is {len(input_files)}")

        for src_file in input_files:
            src_count = ParamsGenerator.process_single_file(src_file, out_src_fd, self.in_start_src_line, self.end_src_line)
            hdr_count = ParamsGenerator.process_single_file(src_file, out_hdr_fd, self.start_hdr_line, self.end_hdr_line)
            if src_count == hdr_count:
                log_info(f"{src_file} has {src_count} params.")
            else:
                log_warn(f"{src_file} is broken: {src_count} or {hdr_count} params")

        self.finish_src_file(out_src_fd)
        self.finish_hdr_file(out_hdr_fd)

class CGenerator(ParamsGenerator):
    def __init__(self, output_file_name) -> None:
        super().__init__()
        self.input_param_file_name = "*_params.c"
        self.generated_source_file_name = f"{output_file_name}.c"
        self.generated_header_file_name = f"{output_file_name}.h"
        self.end_hdr_line = "} IntParamsIndexes;"
        self.include_storage_line = "#include \"storage.h\"\n\n"
        self.start_hdr_line = "typedef enum {"

class CppGenerator(ParamsGenerator):
    def __init__(self, output_file_name) -> None:
        super().__init__()
        self.input_param_file_name = "*_params.cpp"
        self.generated_source_file_name = f"{output_file_name}.cpp"
        self.generated_header_file_name = f"{output_file_name}.hpp"
        self.end_hdr_line = "};"
        self.include_storage_line = "extern \"C\" {\n    #include \"storage.h\"\n}\n\n"
        self.start_hdr_line = "enum IntParamsIndexes : ParamIndex_t {"

def print_help():
    usage_line = "python3 params_generate_array.py [input_dir] [output_dir] [language: `c++` or `c`] [output_file_name]"
    short_desc_line = "Generate a single C++ array and enum class with parameters based on several inputs."
    example_desc_line = "Example: python3 params_generate_array.py ./Src/ ./ c++ params"
    print(f"{usage_line}\n    {short_desc_line}\n    {example_desc_line}")


def parse_args():
    num_of_args = len(sys.argv)
    if num_of_args != 5:
        log_err("Wrong args number")
        print_help()
        exit()

    input_dir = sys.argv[1]
    if not os.path.exists(input_dir):
        log_err(f"Input file with paths `{input_dir}` is not exist!")
        exit()

    output_dir = sys.argv[2]
    if not os.path.exists(output_dir):
        log_err(f"Output directory `{output_dir}` is not exist!")
        exit()

    language = sys.argv[3]
    if language == "c":
        language = LANGUAGE_C
    elif language == "c++":
        language = LANGUAGE_CPP
    else:
        log_err(f"Language `{language}` is not supported!")
        exit()

    output_file_name = sys.argv[4]
    if output_file_name is None or len(output_file_name) == 0:
        log_err(f"Output file name is empty!")
        exit()

    print("Config of the parameters generator:")
    print(f"- Input file with paths: `{input_dir}`")
    print(f"- Output directory: `{output_dir}`")
    print(f"- Language: `{sys.argv[3]}`")
    print(f"- Output file name: `{output_file_name}`")

    return input_dir, output_dir, language, output_file_name

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
    input_dir, output_dir, language, output_file_name = parse_args()

    if language == LANGUAGE_C:
        generator = CGenerator(output_file_name)
    elif language == LANGUAGE_CPP:
        generator = CppGenerator(output_file_name)
    else:
        exit()

    src_files = find_src_files(input_dir, generator)
    generator.generate_everything(src_files, output_dir)
