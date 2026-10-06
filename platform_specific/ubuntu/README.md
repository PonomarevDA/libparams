# Ubuntu flash driver (SITL)

The driver emulates MCU flash in RAM and persists it to YAML files, so a libparams application runs on Linux or macOS.
The unit tests use it as a mock flash.

The emulated page is 2048 bytes; the number of pages follows from the number of parameters.

## Parameter files

| Setting | Runtime | Build time |
| ------- | ------- | ---------- |
| Directory | `paramsSetDir(dir)` before `paramsInit()` | `-DLIBPARAMS_PARAMS_DIR="dir"` |
| Default values, read-only | | `-DLIBPARAMS_DEFAULT_PARAMS_FILE_NAME="default_params"` |
| Saved values | | `-DLIBPARAMS_NVM_PARAMS_FILE_NAME="nvm_params"` |

On init the driver reads `<dir>/nvm_params_<page>.yaml` and falls back to `<dir>/default_params_<page>.yaml`.
`paramsSave()` writes `nvm_params_<page>.yaml`.

Generate the default files from the generated `params.cpp`:

```bash
./scripts/generate_default_params.py --out-dir <dir> -f params.cpp --out-file-name default_params
```

`LIBPARAMS_INIT_PARAMS_FILE_NAME` and `LIBPARAMS_TEMP_PARAMS_FILE_NAME` are deprecated aliases of the two file names.

## Logging

| Level | Messages |
| ----- | -------- |
| `LIBPARAMS_LOG_NONE` | Nothing |
| `LIBPARAMS_LOG_ERROR` | Errors, to stderr |
| `LIBPARAMS_LOG_INFO` (default) | Params dir, files read, changed values |
| `LIBPARAMS_LOG_DEBUG` | Full parameter table on load, every flash erase and write |

- Build time: `-DLIBPARAMS_LOG_LEVEL=1` keeps only errors.
- Runtime: `ubuntuFlashSetLogLevel(LIBPARAMS_LOG_ERROR)`, before `paramsInit()` to quiet the initial load.
- Own logger: `ubuntuFlashSetLogHandler(handler)` routes the messages that pass the level filter; `NULL` restores stdout/stderr.

```c++
static void onLibparamsLog(LibparamsLogLevel level, const char* message) {
    // Forward to the application logger.
}

ubuntuFlashSetLogHandler(onLibparamsLog);
```

See [platform_flash_driver.h](platform_flash_driver.h).

## Examples

- [tests/platform_specific/ubuntu](../../tests/platform_specific/ubuntu): minimal application.
- [tests/params_generator](../../tests/params_generator): generated C and C++ parameters.
- [tests/unit_tests](../../tests/unit_tests): unit tests.
