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
|    STM32f1   | |    STM32g0   | |    Ubuntu    |
| Flash Driver | | Flash Driver | | SITL Driver  |
+--------------+ +--------------+ +--------------+
```

The library is suitable for Cyphal, DroneCAN and other applications where persistent storage is required.

## 1. High level interface. Parameters

There are 4 types of parameters that we may want to store (they are defined in [ParamType_t](https://github.com/PonomarevDA/libparams/blob/585dd09fdd3267675acdf890978c1a266b38c39a/libparams/storage.h#L29) enum):
- PARAM_TYPE_INTEGER (int32)
- PARAM_TYPE_REAL (float32)
- PARAM_TYPE_BOOLEAN (uint8)
- PARAM_TYPE_STRING (uint8[56])

Each parameter has at least the following fields:
- value
- default value (immutable, that means in can't be changed in real time)
- flags: mutability, persistence, visability.

The parameter properties were inspired by the register properties in [Cyphal specification](https://github.com/OpenCyphal/public_regulated_data_types/blob/master/uavcan/register/384.Access.1.0.dsdl). The following properties can be configured by the design of the library:

| Property    | Meaning |
| ----------- | ------- |
| Mutability  | Mutability defines the write access. If the parameter is mutable, it can be written by a user for example via Cyphal register interface. Immutable paramters are not allowed to be written by such services, but that doesn't imply that their values are constant (unchanging), because internally the application still can do it. |
| Persistence </br> (not yet*) | Persistence means that the parameter retains its value permanently across power cycles or any other changes in the state of the server, until it is explicitly overwritten. |
| Visability </br> (not yet*) | If the parameter is visable, it means it can be accesed from the outside of the application. If the parameter is hidden, it is expected to use it for interal purposes only. The library itself doesn't rely on this property. It is reserved for higher level purposes. |

> At the moment, all parameters are always persistent and visible. The only property you can configure is mutability.

Beside the properties above the Integer and Real parameters have the following additional fields:
- min (const)
- max (const)

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

1. [STM32F103 (128 Kbytes) flash driver](platform_specific/stm32f103/README.md)
2. [STM32G0B1xx (512 Kbytes flash driver)](platform_specific/stm32g0b1/README.md)
3. [Ubuntu flash memory emulation (using a file)](platform_specific/ubuntu/README.md)

For implementation details please refer to the corresponded folder.

New drivers might be added in future.

## 4. Usage example

Before using the library, you need to define the parameters first.

You can create params.c and params.h files with the following content:

```c++
// params.h
#pragma once
#include "storage.h"

enum IntParamsIndexes : ParamIndex_t {
    PARAM_NODE_ID,
    PARAM_MAGNETOMETER_ID,
    INTEGER_PARAMS_AMOUNT
};

#define NUM_OF_STR_PARAMS 2
enum class StrParamsIndexes {
    PARAM_SYSTEM_NAME,
    PARAM_MAGNETOMETER_TYPE,
    STRING_PARAMS_AMOUNT
};
```

```c++
// params.c
IntegerDesc_t integer_desc_pool[] = {
    {"uavcan.node.id",      0, 127,     50,     MUTABLE},
    {"uavcan.pub.mag.id",   0, 65535,   65535,  MUTABLE},
};
IntegerParamValue_t integer_values_pool[sizeof(integer_desc_pool) / sizeof(IntegerDesc_t)];

StringDesc_t string_desc_pool[] = {
    {"system.name", "", MUTABLE},
    {"uavcan.pub.mag.type", "uavcan.si.sample.magnetic_field_strength.Vector3", IMMUTABLE},

};
StringParamValue_t string_values_pool[sizeof(string_desc_pool) / sizeof(StringDesc_t)];
```

Alternatively, you can define your parameters in yaml file and call transpiler script to generate the files above:

```yaml
# params.yaml
uavcan.node.id:
  type: Integer
  enum: PARAM_NODE_ID
  flags: mutable
  default: 50
  min: 0
  max: 127

system.name:
  type: String
  enum: PARAM_SYSTEM_NAME
  flags: mutable
  default: ""

uavcan.pub.mag:
  type: Port
  data_type: uavcan.si.sample.magnetic_field_strength.Vector3
  enum_base: PARAM_MAGNETOMETER
```

The initialization of the application can be as shown below:

```c++
#include "params.h"

void application_example() {
    paramsInit(IntParamsIndexes::INTEGER_PARAMS_AMOUNT, StrParamsIndexes::STRING_PARAMS_AMOUNT);
    paramsLoadFromFlash();
}
```

Please, refer to the [libparams/storage.h](libparams/storage.h) for the high level usage details because it is self-documented.

More application examples:

- [DroneCAN RL mini v2 example](https://github.com/RaccoonlabDev/mini_v2_node/tree/main/Src/dronecan_application)
- [Cyphal RL mini v2 example](https://github.com/RaccoonlabDev/mini_v2_node/tree/main/Src/cyphal_application)
- [Cyphal ubuntu minimal example](https://github.com/RaccoonlabDev/cyphal_application/tree/devel/examples/ubuntu_minimal)
- [Cyphal ubuntu publisher example](https://github.com/RaccoonlabDev/cyphal_application/tree/devel/examples/ubuntu_publisher_example)
- [Cyphal UAV HITL communicator ubuntu](https://github.com/RaccoonlabDev/cyphal_communicator/tree/master/src)

## How to run SonarCloud Analysis manually:

```bash
export SONAR_TOKEN=<...>
~/Downloads/build-wrapper-linux-x86/build-wrapper-linux-x86-64 --out-dir bw-output make coverage
~/Downloads/sonar-scanner-cli-5.0.1.3006-linux/sonar-scanner-5.0.1.3006-linux/bin/sonar-scanner
```
