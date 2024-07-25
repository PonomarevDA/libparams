[![Quality Gate Status](https://sonarcloud.io/api/project_badges/measure?project=PonomarevDA_libparams&metric=alert_status)](https://sonarcloud.io/summary/new_code?id=PonomarevDA_libparams) [![Coverage](https://sonarcloud.io/api/project_badges/measure?project=PonomarevDA_libparams&metric=coverage)](https://sonarcloud.io/summary/new_code?id=PonomarevDA_libparams) [![Code Smells](https://sonarcloud.io/api/project_badges/measure?project=PonomarevDA_libparams&metric=code_smells)](https://sonarcloud.io/summary/new_code?id=PonomarevDA_libparams) [![Lines of Code](https://sonarcloud.io/api/project_badges/measure?project=PonomarevDA_libparams&metric=ncloc)](https://sonarcloud.io/summary/new_code?id=PonomarevDA_libparams) [![make build](https://github.com/PonomarevDA/libparams/actions/workflows/build.yml/badge.svg)](https://github.com/PonomarevDA/libparams/actions/workflows/build.yml)

# libparams

libparams is a C-written hardware abstract library that consist of 2 things:

- Parameters Storage - an interface under ROM that allows to store integers and strings with O(1) access complexity.
- Abstract ROM Driver - an interface under your flash memory that allows to write and read sequence of bytes.

It also has an example of flash memory driver implementation for stm32f103 (128 KBytes of flash memory), stm32f103g0 (512 KBytes) based on Stm32CubeIDE HAL and simple Ubuntu flash memory emulation (for SITL usege).

**Purpose**

The library is intended for real-time embedded applications with small memory such as stm32f103 where persistent storage is required. It is suitable for Cyphal, DroneCAN and other applications as [Register interface](https://github.com/OpenCyphal/public_regulated_data_types/blob/master/uavcan/register/384.Access.1.0.dsdl).

**Minimum technical requirements**

The codebase is implemented in C99/C11.

The library was tested on stm32f103 (128 Kbytes) and stm32g0 (512 Kbytes). These hardware can be considered as minimum required.

## 1. DESIGN

The design of the library can be illustrated as shown below:

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


### 1.1. High level interface. Parameters

There are 4 types of parameters that we may want to store (they are defined in [ParamType_t](https://github.com/PonomarevDA/libparams/blob/585dd09fdd3267675acdf890978c1a266b38c39a/libparams/storage.h#L29) enum):
- PARAM_TYPE_INTEGER (int32)
- PARAM_TYPE_REAL (float32)
- PARAM_TYPE_BOOLEAN (uint8)
- PARAM_TYPE_STRING (uint8[56])

Each parameter has at least the following fields:
- value
- default value (immutable, that means in can't be changed in real time)
- flags: mutability, required, persistence, visability.

The parameter properties were inspired by the register properties in [Cyphal specification](https://github.com/OpenCyphal/public_regulated_data_types/blob/master/uavcan/register/384.Access.1.0.dsdl). The following properties can be configured by the design of the library:

| Property    | Meaning |
| ----------- | ------- |
| Mutability  | Mutability defines the write access. If the parameter is mutable, it can be written by a user for example via Cyphal register interface. Immutable paramters are not allowed to be written by such services, but that doesn't imply that their values are constant (unchanging), because internally the application still can do it. |
| Required  | Required means that the parameter's default value is not essential. Practically, it means that the paramter will not be reset during the paramsResetToDefault(). |
| Persistence </br> (not yet*) | Persistence means that the parameter retains its value permanently across power cycles or any other changes in the state of the server, until it is explicitly overwritten. |
| Visability </br> (not yet*) | If the parameter is visable, it means it can be accesed from the outside of the application. If the parameter is hidden, it is expected to use it for interal purposes only. The library itself doesn't rely on this property. It is reserved for higher level purposes. |

> At the moment, all parameters are always persistent and visible. The only property you can configure is mutability and required.

Beside the properties above the Integer and Real parameters have the following additional fields:
- min (const)
- max (const)

A parameter of any type is divided into 2 arrays: `*ParamValue_t` (actual values) and `*Desc_t` (auxillary information such as parameter name, default, min and max values) for each parameter type. These arrays expected to be allocated by a user outside the library.

The library allows to have a read/write access to these parameters by their index or name, reset them to default values and other features.

Access to a parameter can be done by index. Since paramters are stored as an array, the access complexity is O(1).

Writing or reading from the external application should be done by name of the parameter. These operations have O(n) complexity, where n - is the total number of the parameters.

Look at [libparams/storage.h](libparams/storage.h) to get full API and [libparams/storage.c](libparams/storage.c) for the implementation details.

### 1.2. Middle level interface. Abstract ROM driver

ROM driver simply allows you to write and read sequence of bytes. Mainly, it consist of 3 operations.

1. Initialization. It is necessary to call `romInit()` to configure the driver. Storage driver do it automatically.

2. Read operation. You just need to call `romRead` with corresponded arguments.

3. Write operation. Writing requires to unlock and lock ROM, so it might be done by calling `romBeginWrite()`, `romWrite` and `romEndWrite()` one by one.

Look at [rom.h](rom.h) to get full API and [rom.c](rom.c) for the implementation details.

### 1.3. Low level interface. Hardware specific flash driver

Although storage and rom drivers are hardware abstract, they still need a hardware related flash driver. Just for an example, here are a few hardware specific flash driver implementations:

1. [STM32F103 (128 Kbytes) flash driver](platform_specific/stm32f103/README.md)
2. [STM32G0B1xx (512 Kbytes flash driver)](platform_specific/stm32g0b1/README.md)
3. [Ubuntu flash memory emulation (using a file)](platform_specific/ubuntu/README.md)

For implementation details please refer to the corresponded folder.

New drivers might be added in future.

## 2. USAGE

It is expected either to add the library into your project as submodule or just copy the required folders. The library doesn't have external dependencies.

You can add pathes to [libparams](libparams) folder, the folder with parameters and to platform specific flash driver. The application examples in [tests](tests) folder are based on [CMakeLists.txt](CMakeLists.txt).

If it is SITL mode, you need additionally to specify path to yml files with parameters values `LIBPARAMS_PARAMS_DIR`, name of file with initial parameters `LIBPARAMS_INIT_PARAMS_FILE_NAME` and temporal parameters `LIBPARAMS_TEMP_PARAMS_FILE_NAME` ("initial_params" and "temp_params" set as default).

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
    paramsInit(IntParamsIndexes::INTEGER_PARAMS_AMOUNT, StrParamsIndexes::STRING_PARAMS_AMOUNT, -1, 1);
    paramsLoad();
}
```

Please, refer to the [libparams/storage.h](libparams/storage.h) for the high level usage details because it is self-documented.

## 3. USAGE EXAMPLES

In [tests](tests) folder you can find a few examples about how to use the library:

| Example | Brief descrtiption | How to try |
| ------- | ------------------ | ---------- |
| [params_generator/c](tests/params_generator/c/README.md) | An example how to generate C-parameters with python script | `make c_generator` |
| [params_generator/cpp](tests/params_generator/cpp/README.md) | An example how to generate C++ parameters with python script | `make cpp_generator` |
| [platform_specific/stm32f103](tests/platform_specific/stm32f103/README.md) | Just to test that the library can be build without Warnings and Errors | `make stm32f103` |
| [platform_specific/stm32g0b1](tests/platform_specific/stm32g0b1/README.md) | Just to test that the library can be build without Warnings and Errors | `make stm32g0b1` |
| [platform_specific/ubuntu](tests/platform_specific/ubuntu/README.md) | Just to test that the library can be build without Warnings and Errors | `make ubuntu` |

A few real external applications based on this library:

- [DroneCAN RL mini v2 example](https://github.com/RaccoonlabDev/mini_v2_node/tree/main/Src/dronecan_application)
- [Cyphal RL mini v2 example](https://github.com/RaccoonlabDev/mini_v2_node/tree/main/Src/cyphal_application)
- [Cyphal ubuntu minimal example](https://github.com/RaccoonlabDev/cyphal_application/tree/devel/examples/ubuntu_minimal)
- [Cyphal ubuntu publisher example](https://github.com/RaccoonlabDev/cyphal_application/tree/devel/examples/ubuntu_publisher_example)
- [Cyphal UAV HITL communicator ubuntu](https://github.com/RaccoonlabDev/cyphal_communicator/tree/master/src)

## 4. CONTRIBUTING

Please, follow the [CONTRIBUTING.md](CONTRIBUTING.md) guide.

**How to run SonarCloud Analysis manually**

```bash
export SONAR_TOKEN=<...>
~/Downloads/build-wrapper-linux-x86/build-wrapper-linux-x86-64 --out-dir bw-output make coverage
~/Downloads/sonar-scanner-cli-5.0.1.3006-linux/sonar-scanner-5.0.1.3006-linux/bin/sonar-scanner
```

## 5. LICENSE

The project is distributed under term of MPL v2.0 license.

## 6. ACKNOWLEDGEMENTS

This project has been supported by funds from The Foundation for Assistance to Small Innovative Enterprises (FASIE). Moreover, we are honored to be distinguished as laureates of the "Digital Technologies Code" competition, under the federal project "Digital Technologies". We express our profound gratitude for their invaluable support and endorsement.
