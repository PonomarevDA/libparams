/*
 * Copyright (c) 2022-2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <gtest/gtest.h>
#include <iostream>
#include "rom.h"
#include "libparams_error_codes.h"
#include "flash_driver.h"

class RomDriverMultiplePagesTest : public ::testing::Test {
protected:
    RomDriverInstance rom;

    void SetUp() override {
        rom = romInit(0, flashGetNumberOfPages());
    }
};

// Test Case 1. Initialize ROM Driver Instance
// Test 1.1: Initialize with Single Latest Page, Negative Number
TEST(TestRom, initializeWithSingleLatestPageNegativeNumber) {
    auto rom = romInit(-1, 1);
    ASSERT_TRUE(rom.inited)<< "Failed to init with single latest page using negative number";
}
// Test 1.2: Initialize with Single Latest Page, Positive Number
TEST(TestRom, initializeWithSingleLatestPagePositiveNumber) {
    auto rom = romInit(flashGetNumberOfPages() - 1, 1);
    ASSERT_TRUE(rom.inited) << "Failed to init with single latest page using positive number";
}
// Test 1.3: Initialize with Multiple Pages
TEST_F(RomDriverMultiplePagesTest, initializeWithMultiplePages) {
    ASSERT_TRUE(rom.inited) << "Failed to init with multiple pages";
}
// Test 1.4: Initialize with Invalid Page Index
TEST(TestRom, initializeWithInvalidPageIndex) {
    auto rom = romInit(flashGetNumberOfPages(), 1);
    ASSERT_FALSE(rom.inited) << "Initialized with invalid page index";
}
// Test 1.5: Initialize with Zero Pages
TEST(TestRom, initializeWithZeroPages) {
    auto rom = romInit(-1, 0);
    ASSERT_FALSE(rom.inited) << "Initialized with zero pages";
}


// Test Case 2: Read from ROM
// Test 2.1: Read Data within Bounds
TEST_F(RomDriverMultiplePagesTest, readDataWithingBounds) {
    const auto ROM_SIZE = romGetAvailableMemory(&rom);
    uint8_t data[ROM_SIZE];

    // Read a chank of data
    ASSERT_EQ(romRead(&rom, 0, data, 8), 8) << "Failed to read data within bounds";

    // Read everything
    ASSERT_EQ(romRead(&rom, 0, data, ROM_SIZE), ROM_SIZE) << "Failed to read entire ROM size";
}
// Test 2.2: Read Data Exceeding Bounds
TEST_F(RomDriverMultiplePagesTest, readDataExceedingBounds) {
    const auto ROM_SIZE = romGetAvailableMemory(&rom);
    uint8_t data[ROM_SIZE];

    // Normal, clamped read
    ASSERT_EQ(romRead(&rom, 0, data, ROM_SIZE + 1), ROM_SIZE) << "Failed to clamp read";
    ASSERT_EQ(romRead(&rom, 1, data, ROM_SIZE), ROM_SIZE - 1) << "Failed to clamp read";

    // Wrong inputs
    ASSERT_EQ(romRead(nullptr, 0, data, ROM_SIZE), 0) << "Read should fail with nullptr ROM";
    ASSERT_EQ(romRead(&rom, 0, nullptr, ROM_SIZE), 0) << "Read should fail with nullptr data";
    ASSERT_EQ(romRead(&rom, ROM_SIZE, data, 1), 0) << "Read should fail: offset out of bounds";
    ASSERT_EQ(romRead(&rom, 0, data, 0), 0) << "Read should fail with zero size";
}

// Test Case 3: Write to ROM
// Test 3.1: Write Data within Bounds
TEST_F(RomDriverMultiplePagesTest, writeDataWithingBounds) {
    const uint8_t SAMPLE_DATA[] = {1, 2, 3, 4, 5, 6, 7, 8};
    uint8_t read_data[sizeof(SAMPLE_DATA)] = {};

    romBeginWrite(&rom);
    ASSERT_EQ(romWrite(&rom, 0, SAMPLE_DATA, sizeof(SAMPLE_DATA)), sizeof(SAMPLE_DATA));
    romEndWrite(&rom);

    ASSERT_EQ(romRead(&rom, 0, read_data, sizeof(SAMPLE_DATA)), sizeof(SAMPLE_DATA));
    ASSERT_EQ(memcmp(SAMPLE_DATA, read_data, sizeof(SAMPLE_DATA)), 0);
}
// Test 3.2: Write Data Exceeding Bounds
TEST_F(RomDriverMultiplePagesTest, writeDataExceedingBounds) {
    const uint8_t SAMPLE_DATA[] = {1, 2, 3, 4, 5, 6, 7, 8};

    // 1. ROM nullptr
    romBeginWrite(&rom);
    ASSERT_EQ(romWrite(nullptr, 0, SAMPLE_DATA, sizeof(SAMPLE_DATA)), LIBPARAMS_ROM_WRITE_BAD_ARGS_ERROR);
    romEndWrite(&rom);

    // 2. Offset out of bound
    ASSERT_EQ(romWrite(&rom, rom.total_size, SAMPLE_DATA, sizeof(SAMPLE_DATA)), LIBPARAMS_ROM_WRITE_BAD_ARGS_ERROR);

    // 3. Data nullptr
    romBeginWrite(&rom);
    ASSERT_EQ(romWrite(&rom, 0, nullptr, sizeof(SAMPLE_DATA)), LIBPARAMS_ROM_WRITE_BAD_ARGS_ERROR);
    romEndWrite(&rom);

    // 4. Zero wring size
    romBeginWrite(&rom);
    ASSERT_EQ(romWrite(&rom, 0, SAMPLE_DATA, 0), LIBPARAMS_ROM_WRITE_BAD_ARGS_ERROR);
    romEndWrite(&rom);

    // 5. Offset and size out of bound
    romBeginWrite(&rom);
    ASSERT_EQ(romWrite(&rom, 3000, SAMPLE_DATA, 3000), LIBPARAMS_ROM_WRITE_BAD_ARGS_ERROR);
    romEndWrite(&rom);

    // 6. Without romBeginWrite() and romEndWrite()
    ASSERT_EQ(romWrite(&rom, 0, SAMPLE_DATA, sizeof(SAMPLE_DATA)), LIBPARAMS_ROM_WRITE_PROTECTED_ERROR);

    // 7. romBeginWrite() and romEndWrite() nullptr
    romBeginWrite(nullptr);
    ASSERT_EQ(romWrite(&rom, 0, SAMPLE_DATA, sizeof(SAMPLE_DATA)), LIBPARAMS_ROM_WRITE_PROTECTED_ERROR);
    romEndWrite(nullptr);
}

// Test Case 4: Get Available Memory
// Test 4.1: Verify Available Memory Calculation with correct_input
TEST_F(RomDriverMultiplePagesTest, verifyAvaliableMemoryCalculationWithCorrectInput) {
    ASSERT_EQ(romGetAvailableMemory(&rom), flashGetNumberOfPages() * flashGetPageSize());
}
// Test 4.2: Verify Available Memory Calculation with nullptr
TEST_F(RomDriverMultiplePagesTest, test_4_2_verifyAvaliableMemoryCalculationWithNullptr) {
    ASSERT_EQ(romGetAvailableMemory(nullptr), 0);
}

int main (int argc, char *argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
