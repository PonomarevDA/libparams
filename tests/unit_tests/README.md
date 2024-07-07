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

# Storage driver

## Test Case 1: Initialization of Parameters

- Test 1.1: Initialize with Valid Inputs
- Test 1.2: Initialize with zero params
- Test 1.3: Initialize with too much params
- Test 1.4: Initialize with Zero Pages
- Test 1.5: Initialize with Invalid Page Index

## Test Case 2: Load Parameters
- Test 2.1: Load Parameters Successfully
- Test 2.2: Load Parameters with Uninitialized Parameters

## Test Case 3: Save Parameters
- Test 3.1: Save Parameters Successfully
- Test 3.2: Save Parameters with Uninitialized Parameters

## Test Case 4: Reset Parameters to Default
- Test 4.1: Reset Parameters Successfully

## Test Case 5: Access Parameter Names
- Test 5.1: Get Parameter Name Successfully
- Test 5.2: Get Parameter Name with Invalid Index

## Test Case 6: Find Parameter by Name
- Test 6.1: Find Parameter by Valid Name
- Test 6.2: Find Parameter by Invalid Name

## Test Case 7: Access Parameter Types
- Test 7.1: Get Parameter Type Successfully
- Test 7.2: Get Parameter Type with Invalid Index

## Test Case 8: Access Parameter Descriptions
- Test 8.1: Get Integer Parameter Description Successfully
- Test 8.2: Get Integer Parameter Description with Invalid Index
- Test 8.3: Get String Parameter Description Successfully
- Test 8.4: Get String Parameter Description with Invalid Index

## Test Case 9: Access Parameter Values
- Test 9.1: Get Integer Parameter Value Successfully
- Test 9.2: Get Integer Parameter Value with Invalid Index
- Test 9.3: Get String Parameter Value Successfully
- Test 9.4: Get String Parameter Value with Invalid Index

## Test Case 10: Set Parameter Values
- Test 10.1: Set Integer Parameter Value Successfully
- Test 10.2: Set Integer Parameter Value with Invalid Index
- Test 10.3: Set String Parameter Value Successfully
- Test 10.4: Set String Parameter Value with Invalid Index


# ROM driver

## Test Case 1: Initialize ROM Driver Instance

- Test 1.1: Initialize with Single Latest Page, Negative Number
- Test 1.2: Initialize with Single Latest Page, Positive Number
- Test 1.3: Initialize with Multiple Pages
- Test 1.4: Initialize with Invalid Page Index
- Test 1.5: Initialize with Zero Pages

## Test Case 2: Read from ROM

- Test 2.1: Read Data within Bounds
- Test 2.2: Read Data Exceeding Bounds

## Test Case 3: Write to ROM

- Test 3.1: Write Data within Bounds
- Test 3.2: Write Data Exceeding Bounds

## Test Case 4: Get Available Memory

- Test 4.1: Verify Available Memory Calculation
- Test 4.2: Verify Available Memory Calculation with nullptr
