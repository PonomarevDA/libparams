[![Quality Gate Status](https://sonarcloud.io/api/project_badges/measure?project=PonomarevDA_libparams&metric=alert_status)](https://sonarcloud.io/summary/new_code?id=PonomarevDA_libparams) [![Coverage](https://sonarcloud.io/api/project_badges/measure?project=PonomarevDA_libparams&metric=coverage)](https://sonarcloud.io/summary/new_code?id=PonomarevDA_libparams) [![Code Smells](https://sonarcloud.io/api/project_badges/measure?project=PonomarevDA_libparams&metric=code_smells)](https://sonarcloud.io/summary/new_code?id=PonomarevDA_libparams) [![Lines of Code](https://sonarcloud.io/api/project_badges/measure?project=PonomarevDA_libparams&metric=ncloc)](https://sonarcloud.io/summary/new_code?id=PonomarevDA_libparams) [![CI](https://github.com/PonomarevDA/libparams/actions/workflows/ci.yml/badge.svg)](https://github.com/PonomarevDA/libparams/actions/workflows/ci.yml)

# libparams

libparams is a C library for persistent parameter storage in real-time embedded applications.
It keeps integer and string parameters in RAM with O(1) access by index and stores them in flash through a small driver interface.

- The core is C11 with no heap and no external dependencies.
- Parameters are declared in YAML; the C/C++ tables are generated at build time.
- Optional redundant flash pages, CRC check and erased-flash detection.
- Runs on STM32 microcontrollers and on Linux/macOS, where flash is emulated by YAML files (SITL).

## 1. Design

```
+------------------------------------------------+
|        Parameters (storage.h, storage.c)       |
+------------------------------------------------+
                        |
+------------------------------------------------+
|         Abstract ROM driver (rom.h, rom.c)     |
+------------------------------------------------+
                        | FlashDriverOps
+-----------+-----------+-------------+----------+
| STM32F103 | STM32G0B1 | STM32H753xx |  Ubuntu  |
|   flash   |   flash   | flash, FRAM |  (SITL)  |
+-----------+-----------+-------------+----------+
```

### 1.1. Parameters

Two parameter types are implemented:

| Type | Value | Descriptor fields |
| ---- | ----- | ----------------- |
| Integer | `int32_t` | name, min, max, default, mutability, required |
| String | `uint8_t[56]` | name, default, mutability |

| Property | Meaning |
| -------- | ------- |
| Mutability | An immutable parameter can't be changed through `paramsSet*()`. A string parameter that is immutable always reads its default. |
| Required | `paramsResetToDefault()` keeps the value of a required immutable integer parameter. |

Values and descriptors are arrays defined by the application (usually generated, see [2.1](#21-parameters-from-yaml)): `integer_values_pool`, `integer_desc_pool`, `string_values_pool`, `string_desc_pool`.
Integers come first, so a string parameter index starts at `INTEGER_PARAMS_AMOUNT`.

Access by index is O(1).
Lookup by name (`paramsFind()`) is O(n), where n is the number of parameters.

The full API is documented in [include/libparams/storage.h](include/libparams/storage.h).

### 1.2. Abstract ROM driver

The ROM driver reads and writes a byte sequence within a range of flash pages: `romInit()`, `romRead()`, then `romBeginWrite()`, `romWrite()`, `romEndWrite()`.
The storage layer calls it for you.
See [include/libparams/rom.h](include/libparams/rom.h).

### 1.3. Flash drivers

A flash driver is a `FlashDriverOps` table ([flash_driver.h](include/libparams/flash_driver.h)) returned by `<platform>GetOps()`.

| Platform | Ops | Notes |
| -------- | --- | ----- |
| [stm32f103](platform_specific/stm32f103/README.md) | `stm32f103InternalFlashGetOps()` | 128 KB internal flash, HAL |
| [stm32g0b1](platform_specific/stm32g0b1/README.md) | `stm32g0b1InternalFlashGetOps()` | 512 KB internal flash, HAL |
| [stm32h753xx](platform_specific/stm32h753xx/README.md) | `stm32h753xxInternalFlashGetOps()`, `stm32h753xxSpiFramGetOps()` | 2 MB internal flash or external SPI FRAM |
| [ubuntu](platform_specific/ubuntu/README.md) | `ubuntuFlashGetOps()` | Flash emulated in RAM, persisted to YAML files |

## 2. Usage

### 2.1. Parameters from YAML

```yaml
# params.yaml
node.id:
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
  default: "libparams"
```

The generator turns one or more YAML files into `params.cpp` (descriptor and value arrays) and `params.hpp` (`IntParamsIndexes`, `StrParamsIndexes`, `NUM_OF_STR_PARAMS`), or `params.c` and `params.h` for C.

### 2.2. Build

Add the repository as a submodule and include [libparams.cmake](libparams.cmake):

```cmake
set(LIBPARAMS_PLATFORM stm32g0b1)  # stm32f103, stm32g0b1, stm32h753xx, ubuntu
include(${LIBPARAMS_PATH}/libparams.cmake)

libparams_generate_params(OUT_DIR ${CMAKE_BINARY_DIR} YAML params.yaml)

add_executable(app main.cpp ${CMAKE_BINARY_DIR}/params.cpp ${libparamsSrc})
target_include_directories(app PRIVATE ${libparamsHeaders} ${CMAKE_BINARY_DIR})
```

`libparams_generate_params()` runs the generator at configure time and re-configures when a YAML file changes.
It also takes `LANGUAGE c`, `DEFAULT_PARAMS_DIR` for SITL default values and `DOCS` for a markdown table, see [libparams.cmake](libparams.cmake).
Prefer it over calling the scripts directly: the scripts are an implementation detail.

### 2.3. Application

```c++
#include "storage.h"
#include "platform_flash_driver.h"
#include "params.hpp"

int main() {
    // Use the last flash page.
    paramsInit(stm32g0b1InternalFlashGetOps(), INTEGER_PARAMS_AMOUNT, NUM_OF_STR_PARAMS, -1, 1);
    paramsLoad();

    paramsSetIntegerValue(PARAM_NODE_ID, 42);
    paramsSave();
}
```

For SITL builds (`LIBPARAMS_PLATFORM=ubuntu`), set the YAML directory and the log level as described in [platform_specific/ubuntu](platform_specific/ubuntu/README.md).

## 3. Examples and tests

| Example | Description | Command |
| ------- | ----------- | ------- |
| [tests/params_generator/c](tests/params_generator/c/README.md) | Generate C parameters | `make c_generator` |
| [tests/params_generator/cpp](tests/params_generator/cpp/README.md) | Generate C++ parameters | `make cpp_generator` |
| [tests/params_generator/smoke](tests/params_generator/smoke) | `libparams_generate_params()` in C and C++: load SITL defaults, save, load again | `make generator_smoke` |
| [tests/platform_specific](tests/platform_specific) | Build for a platform without warnings | `make stm32f103`, `make stm32g0b1`, `make stm32h753xx`, `make ubuntu` |
| [tests/unit_tests](tests/unit_tests/README.md) | Unit tests, requires GoogleTest | `make unit_tests` |

Firmware based on this library: [node-core](https://github.com/RaccoonlabDev/mini_v2_node).

## 4. Footprint

Core library (`src/rom.c`, `src/storage.c`), `arm-none-eabi-gcc` 15.3, `-Os -mthumb`:

| Core | Flash | RAM |
| ---- | ----- | --- |
| Cortex-M0+ | 2731 B | 326 B |
| Cortex-M3 | 2741 B | 326 B |
| Cortex-M7 | 2741 B | 326 B |

256 B of RAM is the SITL directory buffer.
Flash drivers and the generated tables are not included.

Cost per parameter:

| Type | RAM | Flash storage |
| ---- | --- | ------------- |
| Integer | 24 B: 4 B value, 20 B descriptor | 4 B |
| String | 120 B: 56 B value, 64 B descriptor | 56 B |

For example, 100 integers and 10 strings take 960 B of flash storage and fit in a single 2 KB page.

Reproduce with `make footprint`.
