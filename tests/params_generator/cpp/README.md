# C++ parameters generator

This is the simplest Ubuntu example with CMake.

## Purpose

The application is inbtended to check that the python script can correctly generate the paramters. It can be also used as an example how to write your own parameters.

## Usage

```bash
cd libparams
make cpp_generator
```

It will generate generate C++ parameters from [baro.yaml](baro.yaml) and [mag.yaml](mag.yaml) to `libparams/build` folder, build an executable for Ubuntu application and run it. 

## Notes

- Since Ubuntu flash memory drover emulates only a single 2 Kbyte page, we call `romInit(0, 1)`.
- Related workflow: [![build](https://github.com/PonomarevDA/libparams/actions/workflows/build.yml/badge.svg)](https://github.com/PonomarevDA/libparams/actions/workflows/build.yml)
