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

A parameter of any type is divided into 2 arrays: `*ParamValue_t` (actual values) and `*Desc_t`. Look at [storage.h](storage.h) files for API and [storage.c](storage.c) for the implementation details.

## 2. Middle level interface. Abstract ROM driver

ROM driver simply allows you to write and read sequence of bytes. Mainly, it consist of 3 operations.

1. Initialization. It is necessary to call `romInit()` to configure the driver. Storage driver do it automatically.

2. Read operation. You just need to call `romRead` with corresponded arguments.

3. Write operation. Writing requires to unlock and lock ROM, so it might be done by calling `romBeginWrite()`, `romWrite` and `romEndWrite()` one by one.

Look at [rom.h](rom.h) to get full API and [rom.c](rom.c) for the implementation details.

## 3. Low level interface. Hardware specific flash driver

Here there are 2 flash drivers implementation:

<details>
  <summary>1. STM32F103</summary>

According to [the reference manual](https://www.st.com/resource/en/reference_manual/cd00171190-stm32f101xx-stm32f102xx-stm32f103xx-stm32f105xx-and-stm32f107xx-advanced-arm-based-32-bit-mcus-stmicroelectronics.pdf) STM32F1 has different page size depends on flash size:

| Name | Series | Flash density, KB | Page size, KB |
| ---- | ------ | ----------------- | ------------- |
| Low-density | STM32F101xx, STM32F102xx, STM32F103xx | 16 - 32 | 1 |
| Medium-density | STM32F101xx, STM32F102xx, STM32F103xx | 64 - 128 | 1 |
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
  <summary>2. STM32G0</summary>
    not ready yet
</details>


The library allocates the last page by default.

## 4. Usage example

To use the library you need to provide `IntegerDesc_t integer_desc_pool[]` and `StringDesc_t string_desc_pool[]` arrays with parameters.

```c++
#include "params.hpp"
#include <assert.h>

/**
 * @note Names of these params should not contain spaces, because Mavlink console can't handle them
 */
IntegerDesc_t integer_desc_pool[] = {
    // name                         val     min     max     default
    {(uint8_t*)"identifier",        50,     0,      100,    50},
    {(uint8_t*)"log_level",         3,      0,      4,      3},
}

StringDesc_t string_desc_pool[] = {
    // name                         val             default
    {(uint8_t*)"name",              "custom_name",  "default_name"},
};

static_assert(sizeof(integer_desc_pool) + sizeof(string_desc_pool) < PAGE_SIZE_BYTES, "Parameters are out of flash.");
```

Then you need to call to initialize the library:

```c++
paramsInit(int_params_amount, str_params_amount);
```

Now the library is ready to use.
