# libparams

This library is dedicated for storing parameters on stm32f103 flash memory.

It is useful for any STM32 application, especially for those which are based on Cyphal/DroneCAN.

> The library expect stm32f103 with 128 KBytes of flash, but it might be easily adopted for other sizes.

## Low level description

According to the datasheets STM32F1 has different page size depends on flash size:

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

The library allocates the last page.

On the low level it suggests 256 words.

We can read a 32-bit word from flash memory at any time.

To write something to flash memory we need:
1. unlock the flash memory
2. erase the flash memory
3. write
4. lock the flash memory

Look at [flash.h](flash.h) files for API and [flash.c](flash.c) for the implementation details.

## High level description

There are 4 types of parameters that we may want to store:
- integer (int32)
- real (float32)
- boolean (uint8)
- string (uint8[20])

Each integer/real parameter has following fields:
- value (volatile)
- default value (hardcoded)
- min (hardcoded)
- max (hardcoded)

Each string/boolean parameter has following fields:
- value
- default value (hardcoded)

The library stores in flash storage only the actual value.

Look at [storage.h](storage.h) files for API and [storage.c](storage.c) for the implementation details.

## Usage example

To use the library you need to provide `IntegerCell_t parameters[]` and `StringCell_t str_params[]` arrays with parameters.

```c++
#include "params.h"
#include <assert.h>

/**
 * @note Names of these params should not contain spaces, because Mavlink console can't handle them
 */
IntegerCell_t parameters[] = {
    // name                         val     min     max     default
    {(uint8_t*)"identifier",        50,     0,      100,    50},
    {(uint8_t*)"log_level",         3,      0,      4,      3},
}

StringCell_t str_params[] = {
    // name                         val             default
    {(uint8_t*)"name",              "custom_name",  "default_name"},
};

static_assert(sizeof(parameters) + sizeof(str_params) < PAGE_SIZE_BYTES, "Parameters are out of flash.");
```

Then you need to call to initialize the library:

```c++
paramsInit(int_params_amount, str_params_amount);
```

Now the library is ready to use.
