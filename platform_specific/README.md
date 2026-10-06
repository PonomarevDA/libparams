# Platform specific

Flash drivers implementing `FlashDriverOps` from [flash_driver.h](../include/libparams/flash_driver.h):

- [stm32f103](stm32f103/README.md): internal flash, HAL.
- [stm32g0b1](stm32g0b1/README.md): internal flash, HAL.
- [stm32h753xx](stm32h753xx/README.md): internal flash or external SPI FRAM, HAL.
- [ubuntu](ubuntu/README.md): flash emulated in RAM and persisted to YAML files, for SITL and unit tests.
