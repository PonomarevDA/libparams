# Ubuntu stm32 Flash Driver emulator

The Ubuntu flash driver emulates a real flash memory of an stm32 microcontroller.

The implementation is quite simple: it allocates an array with the size multiple of a typical stm32 page (2048 bytes). The number of flash pages are calculated based on number of parameters.

If a strings named `LIBPARAMS_PARAMS_DIR` is defined, the driver will read initial parameters values from located in the directory yaml files into the allocated array during the initialization.
> The default file names are "init_params_n.yaml", where n -- the number of flash pages. Using the `LIBPARAMS_INIT_PARAMS_BASE_NAME` you can specify the file name, such that the library will search for `${LIBPARAMS_PARAMS_DIR}/${LIBPARAMS_INIT_PARAMS_BASE_NAME}_n.yaml` file for each flash page.

> The `LIBPARAMS_TEMP_PARAMS_BASE_NAME` specifies the names for files where the parameters will be saved during the `flashWrite` function call.

The Ubuntu flash driver allows you to run an application based on libparams on Ubuntu. For example, it allows you to run a Cyphal/DroneCAN SITL node.

This driver is also used as a mock driver for hardware related things to provide unit tests for storage and rom drivers.

The generator tests are based on this driver:
- [tests/params_generator/c](../../tests/params_generator/c/)
- [tests/params_generator/cpp](../../tests/params_generator/cpp/)

The unit tests are based on this driver:
- [tests/unit_tests](../../tests/unit_tests/)

An example of a simple Ubuntu application:
- [tests/platform_specific/ubuntu](../../tests/platform_specific/ubuntu)
