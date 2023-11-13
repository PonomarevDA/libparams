# Ubuntu stm32 Flash Driver emulator

The Ubuntu flash driver emulates a real flash memory of an stm32 microcontroller.

The implementation is quite simple: it allocates an array with the size of a typical stm32 page (2048 bytes). It is expected that a single page will be sufficient.

If a string named `FLASH_DRIVER_STORAGE_FILE` is defined, the driver will read the yaml file into the allocated array during the initialization.

The Ubuntu flash driver allows you to run an application based on libparams on Ubuntu. For example, it allows you to run a Cyphal/DroneCAN SITL node.

This driver is also used as a mock driver for hardware related things to provide unit tests for storage and rom drivers.

There are a few tests based on this driver:
- [tests/unit_tests](../../tests/unit_tests/)
- [tests/params_generator/c](../../tests/params_generator/c/)
- [tests/params_generator/cpp](../../tests/params_generator/cpp/)
