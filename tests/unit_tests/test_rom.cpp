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

// Test Case 1. Initialize ROM Driver Instance
// Test 1.1: Initialize with Single Latest Page, Negative Number
TEST(TestRom, test_1_1_initialize_with_single_latest_page_negative_number) {
    auto rom = romInit(-1, 1);
    ASSERT_TRUE(rom.inited);
}
// Test 1.2: Initialize with Single Latest Page, Positive Number
TEST(TestRom, test_1_2_initialize_with_single_latest_page_positive_number) {
    auto rom = romInit(flashGetNumberOfPages() - 1, 1);
    ASSERT_TRUE(rom.inited);
}
// Test 1.3: Initialize with Multiple Pages
TEST(TestRom, test_1_3_initialize_with_multiple_pages) {
    auto rom = romInit(0, flashGetNumberOfPages());
    ASSERT_TRUE(rom.inited);
}
// Test 1.4: Initialize with Invalid Page Index
TEST(TestRom, test_1_4_initialize_with_invalid_page_index) {
    auto rom = romInit(flashGetNumberOfPages(), 1);
    ASSERT_FALSE(rom.inited);
}
// Test 1.5: Initialize with Zero Pages
TEST(TestRom, test_1_5_initialize_with_zero_pages) {
    auto rom = romInit(-1, 0);
    ASSERT_FALSE(rom.inited);
}


// Test Case 2: Read from ROM
// Test 2.1: Read Data within Bounds
TEST(TestRom, test_2_1_read_data_withing_bounds) {
    auto rom = romInit(0, flashGetNumberOfPages());
    const auto ROM_SIZE = romGetAvailableMemory(&rom);
    uint8_t data[ROM_SIZE];

    // Read a chank of data
    ASSERT_EQ(romRead(&rom, 0, data, 8), 8);

    // Read everything
    ASSERT_EQ(romRead(&rom, 0, data, ROM_SIZE), ROM_SIZE);
}
// Test 2.2: Read Data Exceeding Bounds
TEST(TestRom, test_2_2_read_data_exceeding_bounds) {
    auto rom = romInit(0, flashGetNumberOfPages());
    const auto ROM_SIZE = romGetAvailableMemory(&rom);
    uint8_t data[ROM_SIZE];

    // Normal, clamped read
    ASSERT_EQ(romRead(&rom, 0, data, ROM_SIZE + 1), ROM_SIZE);
    ASSERT_EQ(romRead(&rom, 1, data, ROM_SIZE), ROM_SIZE - 1);

    // Wrong inputs
    ASSERT_EQ(romRead(nullptr, 0, data, ROM_SIZE), 0);
    ASSERT_EQ(romRead(&rom, 0, nullptr, ROM_SIZE), 0);
    ASSERT_EQ(romRead(&rom, ROM_SIZE, data, ROM_SIZE), 0);
    ASSERT_EQ(romRead(&rom, 0, data, 0), 0);
}

// Test Case 3: Write to ROM
// Test 3.1: Write Data within Bounds
TEST(TestRom, test_3_1_write_data_withing_bounds) {
    auto rom = romInit(0, flashGetNumberOfPages());
    const uint8_t SAMPLE_DATA[] = {1, 2, 3, 4, 5, 6, 7, 8};
    uint8_t read_data[sizeof(SAMPLE_DATA)] = {};

    romBeginWrite(&rom);
    ASSERT_EQ(romWrite(&rom, 0, SAMPLE_DATA, sizeof(SAMPLE_DATA)), sizeof(SAMPLE_DATA));
    romEndWrite(&rom);

    ASSERT_EQ(romRead(&rom, 0, read_data, sizeof(SAMPLE_DATA)), sizeof(SAMPLE_DATA));
    ASSERT_EQ(memcmp(SAMPLE_DATA, read_data, sizeof(SAMPLE_DATA)), 0);
}
// Test 3.2: Write Data Exceeding Bounds
TEST(TestRom, test_3_2_romWrite_bad_args) {
    auto rom = romInit(0, flashGetNumberOfPages());
    const uint8_t SAMPLE_DATA[] = {1, 2, 3, 4, 5, 6, 7, 8};

    // 1. ROM nullptr
    romBeginWrite(&rom);
    ASSERT_EQ(romWrite(nullptr, 0, SAMPLE_DATA, sizeof(SAMPLE_DATA)), 0);
    romEndWrite(&rom);

    // 2. Offset out of bound
    ASSERT_EQ(romWrite(&rom, rom.total_size, SAMPLE_DATA, sizeof(SAMPLE_DATA)), 0);

    // 3. Data nullptr
    romBeginWrite(&rom);
    ASSERT_EQ(romWrite(&rom, 0, nullptr, sizeof(SAMPLE_DATA)), 0);
    romEndWrite(&rom);

    // 4. Zero wring size
    romBeginWrite(&rom);
    ASSERT_EQ(romWrite(&rom, 0, SAMPLE_DATA, 0), 0);
    romEndWrite(&rom);

    // 5. Offset and size out of bound
    romBeginWrite(&rom);
    ASSERT_EQ(romWrite(&rom, 1500, SAMPLE_DATA, 1500), 0);
    romEndWrite(&rom);

    // 6. Without romBeginWrite() and romEndWrite()
    ASSERT_EQ(romWrite(&rom, 0, SAMPLE_DATA, sizeof(SAMPLE_DATA)), 0);

    // 7. romBeginWrite() and romEndWrite() nullptr
    romBeginWrite(nullptr);
    ASSERT_EQ(romWrite(&rom, 0, SAMPLE_DATA, sizeof(SAMPLE_DATA)), 0);
    romEndWrite(nullptr);
}

// Test Case 4: Get Available Memory
// Test 4.1: Verify Available Memory Calculation with correct_input
TEST(TestRom, test_4_1_verify_avaliable_memory_calculation_with_correct_input) {
    auto rom = romInit(0, flashGetNumberOfPages());
    ASSERT_EQ(romGetAvailableMemory(&rom), flashGetNumberOfPages() * flashGetPageSize());
}
// Test 4.2: Verify Available Memory Calculation with nullptr
TEST(TestRom, test_4_2_verify_avaliable_memory_calculation_with_nullptr) {
    ASSERT_EQ(romGetAvailableMemory(nullptr), 0);
}

int main (int argc, char *argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
