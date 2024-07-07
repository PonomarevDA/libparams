# Unit tests

Here we have all primaries libparams unit tests.

As well as the application, they are divided into a 3 files:
- test_storage.cpp
- test_rom.cpp
- test_flash_driver.cpp

To run the tests, you should just type:

```bash
make
```

in this folder.

This tests are part of [the build CI](.github/workflows/build.yml).

# ROM driver

## Test Case 1: Initialize ROM Driver Instance

- Test 1.1: Initialize with Single Latest Page, Negative Number
- Test 1.2: Initialize with Single Latest Page, Positive Number
- Test 1.3: Initialize with Multiple Pages
- Test 1.4: Initialize with Invalid Page Index
- Test 1.5: Initialize with Zero Pages

### Test Case 2: Read from ROM

- Test 2.1: Read Data within Bounds
- Test 2.2: Read Data Exceeding Bounds

### Test Case 3: Write to ROM

- Test 3.1: Write Data within Bounds
- Test 3.2: Write Data Exceeding Bounds

### Test Case 4: Get Available Memory

- Test 4.1: Verify Available Memory Calculation
- Test 4.2: Verify Available Memory Calculation with nullptr
