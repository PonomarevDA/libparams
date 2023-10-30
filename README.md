# libparams [![Quality Gate Status](https://sonarcloud.io/api/project_badges/measure?project=PonomarevDA_libparams&metric=alert_status)](https://sonarcloud.io/summary/new_code?id=PonomarevDA_libparams) [![Coverage](https://sonarcloud.io/api/project_badges/measure?project=PonomarevDA_libparams&metric=coverage)](https://sonarcloud.io/summary/new_code?id=PonomarevDA_libparams) [![Code Smells](https://sonarcloud.io/api/project_badges/measure?project=PonomarevDA_libparams&metric=code_smells)](https://sonarcloud.io/summary/new_code?id=PonomarevDA_libparams) [![Lines of Code](https://sonarcloud.io/api/project_badges/measure?project=PonomarevDA_libparams&metric=ncloc)](https://sonarcloud.io/summary/new_code?id=PonomarevDA_libparams)

libparams is a C-written hardware abstract library that consist of 2 things:

- Parameters Storage - an interface under ROM that allows to store integers and strings with O(1) access complexity.
- Abstract ROM Driver - an interface under your flash memory that allows to write and read sequence of bytes.

It also has an example of flash memory driver implementation for stm32f103 based on Stm32CubeIDE HAL.

```
+-------+----------------+---------------+-------+
|       Parameters (storage.c, storage.h)        |
+-------+----------------+---------------+-------+
                         |
+-------+----------------+---------------+-------+
|       Abstract ROM driver (rom.c, rom.h)       |
+-------+----------------+-----------------------+
        |                |                |
+-------+------+ +-------+------+ +-------+------+
|    STM32f1   | |    STM32g0   | |      ...     |
| Flash Driver | | Flash Driver | |              |
+--------------+ +--------------+ +--------------+
```

This library is suitable for Cyphal and DroneCAN applications.

## 1. High level interface. Parameters

There are 4 types of parameters that we may want to store (they are defined in [ParamType_t](https://github.com/PonomarevDA/libparams/blob/585dd09fdd3267675acdf890978c1a266b38c39a/libparams/storage.h#L29) enum):
- PARAM_TYPE_INTEGER (int32)
- PARAM_TYPE_REAL (float32)
- PARAM_TYPE_BOOLEAN (uint8)
- PARAM_TYPE_STRING (uint8[56])

Each integer/real parameter has following fields:
- value
- default value (const)
- min (const)
- max (const)
- flags.is_mutable (const): the parameter can be written from the outside service

Each string/boolean parameter has the following fields:
- value
- default value (const)
- flags.is_mutable (const): the parameter can be written from the outside service

A parameter of any type is divided into 2 arrays: `*ParamValue_t` (actual values) and `*Desc_t` (auxillary information such as parameter name, default, min and max values) for each parameter type. These arrays expected to be allocated by a user outside the library.

The library allows to have a read/write access to these parameters by their index or name, reset them to default values and other features.

Access to a parameter can be done by index. Since paramters are stored as an array, the access complexity is O(1).

Writing or reading from the external application should be done by name of the parameter. These operations have O(n) complexity, where n - is the total number of the parameters.

Look at [libparams/storage.h](libparams/storage.h) to get full API and [libparams/storage.c](libparams/storage.c) for the implementation details.

## 2. Middle level interface. Abstract ROM driver

ROM driver simply allows you to write and read sequence of bytes. Mainly, it consist of 3 operations.

1. Initialization. It is necessary to call `romInit()` to configure the driver. Storage driver do it automatically. The library allocates the last page by default.

2. Read operation. You just need to call `romRead` with corresponded arguments.

3. Write operation. Writing requires to unlock and lock ROM, so it might be done by calling `romBeginWrite()`, `romWrite` and `romEndWrite()` one by one.

Look at [rom.h](rom.h) to get full API and [rom.c](rom.c) for the implementation details.

## 3. Low level interface. Hardware specific flash driver

Although storage and rom drivers are hardware abstract, they still need a hardware related flash driver. Just for an example, here are a few hardware specific flash driver implementations:

1. [STM32F103 (128 Kbytes) flash driver](platform_specific/stm32f103)
2. [STM32G0B1xx (512 Kbytes flash driver)](platform_specific/stm32g0b1)
3. [Ubuntu flash memory emulation using a file)](platform_specific/ubuntu/)

New drivers might be added in future.

## 4. Usage example

A minimal usage example is shown below.

```c++
#include "storage.h"

enum IntParamsIndexes : ParamIndex_t {
    NODE_ID,
    MAGNETOMETER_ID,
    INTEGER_PARAMS_AMOUNT
};

enum class StrParamsIndexes {
    NODE_NAME,
    MAGNETOMETER_TYPE,
    STRING_PARAMS_AMOUNT
};

IntegerDesc_t integer_desc_pool[] = {
    {"uavcan.node.id",      0, 127,     50,     MUTABLE},
    {"uavcan.pub.mag.id",   0, 65535,   65535,  MUTABLE},
};
IntegerParamValue_t integer_values_pool[sizeof(integer_desc_pool) / sizeof(IntegerDesc_t)];

StringDesc_t string_desc_pool[] = {
    {"name",                "Unknown",                                          MUTABLE},
    {"uavcan.pub.mag.type", "uavcan.si.sample.magnetic_field_strength.Vector3", IMMUTABLE},

};
StringParamValue_t string_values_pool[sizeof(string_desc_pool) / sizeof(StringDesc_t)];

void application_example() {
    paramsInit(IntParamsIndexes::INTEGER_PARAMS_AMOUNT, StrParamsIndexes::STRING_PARAMS_AMOUNT);
    paramsLoadFromFlash();
}
```

More application examples:

- [DroneCAN RL mini v2 example](https://github.com/RaccoonlabDev/mini_v2_node/tree/main/Src/dronecan_application)
- [Cyphal RL mini v2 example](https://github.com/RaccoonlabDev/mini_v2_node/tree/main/Src/cyphal_application)
- [Cyphal ubuntu example](https://github.com/RaccoonlabDev/libcanard_cyphal_application)

## How to run SonarCloud Analysis manually:

```bash
export SONAR_TOKEN=<...>
~/Downloads/build-wrapper-linux-x86/build-wrapper-linux-x86-64 --out-dir bw-output make coverage
~/Downloads/sonar-scanner-cli-5.0.1.3006-linux/sonar-scanner-5.0.1.3006-linux/bin/sonar-scanner
```