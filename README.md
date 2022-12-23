# libparams

libparams is a C-written hardware abstract library that consist of 2 things:

- Parameters Storage - an interface under ROM that allows to store integers and strings with O(1) access complexity.
- Abstract ROM Driver - an interface under your flash memory that allows to write and read sequence of bytes.

It also has an example of flash memory driver implementation for stm32f103 based on Stm32CubeIDE HAL.

![scope_of_libparams](doc/scope_of_libparams.png?raw=true "scope_of_libparams")

This library is suitable for Cyphal and DroneCAN application.

> By default the library expect stm32f103 with 128 KBytes of flash, but it might be easily adopted for other sizes.

## 1. High level interface. Parameters

There are 4 types of parameters that we may want to store (they are defined in `StorageParamType_t` enum):
- PARAM_TYPE_INTEGER (int32)
- PARAM_TYPE_REAL (float32)
- PARAM_TYPE_BOOLEAN (uint8)
- PARAM_TYPE_STRING (uint8[20])

Each integer/real parameter has following fields:
- value (volatile)
- default value (hardcoded)
- min (hardcoded)
- max (hardcoded)

Each string/boolean parameter has the following fields:
- value
- default value (hardcoded)

A parameter of any type is divided into 2 arrays: `*ParamValue_t` (actual values) and `*Desc_t`.

Look at [storage.h](storage.h) to get full API and [storage.c](storage.c) for the implementation details.

## 2. Middle level interface. Abstract ROM driver

ROM driver simply allows you to write and read sequence of bytes. Mainly, it consist of 3 operations.

1. Initialization. It is necessary to call `romInit()` to configure the driver. Storage driver do it automatically. The library allocates the last page by default.

2. Read operation. You just need to call `romRead` with corresponded arguments.

3. Write operation. Writing requires to unlock and lock ROM, so it might be done by calling `romBeginWrite()`, `romWrite` and `romEndWrite()` one by one.

Look at [rom.h](rom.h) to get full API and [rom.c](rom.c) for the implementation details.

## 3. Low level interface. Hardware specific flash driver

Here there are following flash drivers implementation:

<details>
  <summary>1. STM32F103</summary>

According to [the reference manual](https://www.st.com/resource/en/reference_manual/cd00171190-stm32f101xx-stm32f102xx-stm32f103xx-stm32f105xx-and-stm32f107xx-advanced-arm-based-32-bit-mcus-stmicroelectronics.pdf) STM32F1 has different page size depends on flash size:

| Name | Series | Flash density, KB | Page size, KB |
| ---- | ------ | ----------------- | ------------- |
| Low-density | STM32F101xx, STM32F102xx, STM32F103xx | 16 - 32 | 1 |
| **Medium-density** | STM32F101xx, STM32F102xx, **STM32F103xx** | **64 - 128** | **1** |
| High-density | STM32F101xx, STM32F103xx | 256 - 512 | 2 |
| XL-density | STM32F101xx, STM32F103xx | 768 - 1024 | 2 |
| Connectivity line devices | STM32F105xx, STM32F107xx | any | 1 |

The memory table for stm32f103 with 128 KBytes might rbe represented as below:

| Name    | hex address               | Size        |
| ------- | ------------------------- | ----------- |
| Page 0  | 0x0800 0000 - 0x0800 03FF | 1 KByte     |
| Page 1  | 0x0800 0400 - 0x0800 07FF | 1 KByte     |
| Page 2  | 0x0800 0800 -             | 1 KByte     |
| Page 4  | 0x0800 1000 -             | 1 KByte     |
| Page 8  | 0x0800 2000 -             | 1 KByte     |
| Page 16 | 0x0800 4000 -             | 1 KByte     |
| Page 32 | 0x0800 8000 -             | 1 KByte     |
| Page 63 | 0x0800 FC00 - 0x0800 FFFF | 1 KByte     |
| Page 64 | 0x0801 0000 -             | 1 KByte     |
| Page 127| 0x0801 FC00 - 0x0801 FCFF | 1 KByte     |
| Page 128| 0x0802 0000 -             | 1 KByte     |

</details>

<details>
  <summary>2. STM32G0B1xx</summary>

According to [the reference manual](https://www.st.com/resource/en/reference_manual/rm0444-stm32g0x1-advanced-armbased-32bit-mcus-stmicroelectronics.pdf) STM32G0B1xx has different page size depends on flash size:

Up to 512 Kbytes of Flash memory (Main memory):
- up to 64 Kbytes for STM32G031xx and STM32G041xx / STM32G051xx and
STM32G061xx
- up to 128 Kbytes for STM32G071xx and STM32G081xx
- **up to 512 Kbytes for STM32G0B1xx** and STM32G0C1xx
- Page size: 2 Kbytes
- Subpage size: 512 bytes

So, we have 512 Kbytes dual-bank device.

The main memory is:

| Name          | hex address               | Size        |
| ------------- | ------------------------- | ----------- |
| Page 383      | 0x0807 F804 - 0x0807 FFFF | 2 KByte     |
| Page 258-382  | ...                       | ...         |
| Page 257      | 0x0804 0800 - 0x0804 0FFF | 2 KByte     |
| Page 256      | 0x0804 0000 - 0x0804 07FF | 2 KByte     |
| Page 127      | 0x0803 F800 - 0x0803 FFFF | 2 KByte     |
| Page 2-126    | ...                       | ...         |
| Page 1        | 0x0800 0800 - 0x0800 0FFF | 2 KByte     |
| Page 0        | 0x0800 0000 - 0x0800 07FF | 2 KByte     |

</details>

New drivers might be added in future.

## 4. Usage example

There are 2 ways of usage of the library:
- You may provide a .cpp and .hpp files with parameters.
- Alternatively, you may provide .yaml files with parameters and generate .cpp and .hpp files based on them.

Let's say you have the following files:

node.yaml:

```yaml
uavcan.node.id   : ["Integer",  "ID",               True,   50,  0,  127]
uavcan.node.name : ["String",   "PARAM_NODE_NAME",  True,   ""]
```

magnetometer.yaml:

```yaml
uavcan.pub.mag.id :   ["Integer", "MAGNETOMETER_ID",    True,  65535,  0,  65535]
uavcan.pub.mag.type : ["String",  "MAGNETOMETER_TYPE",  True, "uavcan.si.sample.magnetic_field_strength.Vector3"]
```

Then call the generator script:

```bash
./libparams/scripts/params_generate_array_from_yaml.py build/ c++ params node.yaml magnetometer.yaml
```

It will create .cpp and .hpp files:

```c++
#include "params.hpp"
extern "C" {
    #include "storage.h"
}
IntegerDesc_t integer_desc_pool[] = {
    {(uint8_t*)"uavcan.node.id", 0, 127, 50},
    {(uint8_t*)"uavcan.pub.mag.id", 0, 65535, 65535},

};
IntegerParamValue_t integer_values_pool[sizeof(integer_desc_pool) / sizeof(IntegerDesc_t)];

StringDesc_t __attribute__((weak)) string_desc_pool[NUM_OF_STR_PARAMS] = {
    {(uint8_t*)"name", "", true},
    {(uint8_t*)"uavcan.pub.mag.type", "uavcan.si.sample.magnetic_field_strength.Vector3", true},

};
StringParamValue_t string_values_pool[sizeof(string_desc_pool) / sizeof(StringDesc_t)];

```

```c++
#pragma once
enum class IntParamsIndexes {
    ID,
    MAGNETOMETER_ID,
    INTEGER_PARAMS_AMOUNT
};

#define NUM_OF_STR_PARAMS 2

```



The minimal usage program might be as follow:

```c++
#include "storage.hpp"
#include "params.hpp"
#include "string_params.hpp"

paramsInit(static_cast<uint8_t>(IntParamsIndexes::INTEGER_PARAMS_AMOUNT), NUM_OF_STR_PARAMS);
paramsLoadFromFlash();
```
