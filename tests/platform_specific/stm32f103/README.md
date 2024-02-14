# stm32f103 (128 Kbytes) example

This is the simplest stm32f103 example with Makefile.

## Purpose

The application is inbtended simply to check that the library can be builded with corresponded compiler for given plathorm without Warnings and Errors.

## Usage


```bash
cd libparams
make stm32f103
```

It will generate .bin file in `libparams/build` folder.

## Notes

- Since we want to utilize the last page of flash memory, we call `romInit(&rom, 127, 1)`.
- Related workflow: [![build](https://github.com/PonomarevDA/libparams/actions/workflows/build.yml/badge.svg)](https://github.com/PonomarevDA/libparams/actions/workflows/build.yml)
