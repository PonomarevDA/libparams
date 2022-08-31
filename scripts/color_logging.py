#!/usr/bin/env python3

class Colors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKCYAN = '\033[96m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'

def log_info(log_str):
    print("INFO: {}".format(log_str))

def log_warn(log_str):
    print("{}WARN: {}{}".format(Colors.WARNING, log_str, Colors.ENDC))

def log_err(log_str):
    print("{}ERR: {}{}".format(Colors.FAIL, log_str, Colors.ENDC))

def log_green(log_str):
    print("{}{}{}".format(Colors.OKGREEN, log_str, Colors.ENDC))

def log_cyan(log_str):
    print("{}{}{}".format(Colors.OKCYAN, log_str, Colors.ENDC))