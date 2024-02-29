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

// RomInit normal usage
TEST(TestRom, test_romInit_normal_first_page) {
    RomDriverInstance rom = romInit(0, 1);
    ASSERT_TRUE(rom.inited);
}
TEST(TestRom, test_romInit_normal_last_page) {
    RomDriverInstance rom = romInit(flashGetNumberOfPages() - 1, 1);
    ASSERT_TRUE(rom.inited);
}
TEST(TestRom, test_romInit_normal_all_pages) {
    RomDriverInstance rom = romInit(0, flashGetNumberOfPages());
    ASSERT_TRUE(rom.inited);
}

// RomInit bad arguments
TEST(TestRom, test_romInit_fail_zero_pages) {
    RomDriverInstance rom = romInit(0, 0);
    ASSERT_FALSE(rom.inited);
}
TEST(TestRom, test_romInit_fail_out_of_memory) {
    RomDriverInstance rom = romInit(flashGetNumberOfPages(), 1);
    ASSERT_FALSE(rom.inited);
}


// romRead normal usage
TEST(TestRom, test_romRead_normal) {
    RomDriverInstance rom = romInit(0, 1);
    uint8_t data[4048];

    // Normal, full read
    ASSERT_EQ(romRead(&rom, 0, data, 8), 8);

    // Normal, clamped read
    ASSERT_EQ(romRead(&rom, 0, data, 4096), 2048);
    ASSERT_EQ(romRead(&rom, 1000, data, 4096), 1048);
    ASSERT_EQ(romRead(&rom, 1000, data, 2048), 1048);
    ASSERT_EQ(romRead(&rom, 1000, data, 2000), 1048);
}

// romRead bad arguments
TEST(TestRom, test_romRead_bad_arguments) {
    RomDriverInstance rom = romInit(0, 1);
    uint8_t data[4048];

    // Wrong inputs
    ASSERT_EQ(romRead(&rom, 0, NULL, 8), 0);
    ASSERT_EQ(romRead(&rom, 4096, data, 8), 0);
    ASSERT_EQ(romRead(&rom, 0, data, 0), 0);

    // Normal, full read
    ASSERT_EQ(romRead(&rom, 0, data, 8), 8);
}


// romWrite normal usage
TEST(TestRom, test_romWrite_normal) {
    RomDriverInstance rom = romInit(0, 1);
    const uint8_t WRITTEN_DATA[] = {1, 2, 3, 4, 5, 6, 7, 8};
    ASSERT_EQ(rom.total_size, flashGetPageSize());

    // Normal inputs, but without romBeginWrite and romEndWrite()
    ASSERT_EQ(romWrite(&rom, 0, WRITTEN_DATA, 8), 0);

    // Normal
    romBeginWrite(&rom);
    size_t write_res = romWrite(&rom, 0, WRITTEN_DATA, 8);
    romEndWrite(&rom);
    ASSERT_EQ(write_res, 8);

    uint8_t read_data[8] = {};
    size_t read_res = romRead(&rom, 0, read_data, 8);
    ASSERT_EQ(read_res, 8);
    int compare_res = memcmp(WRITTEN_DATA, read_data, 8);
    ASSERT_EQ(compare_res, 0);
}
// romWrite bad arguments
TEST(TestRom, test_romWrite_bad_args) {
    RomDriverInstance rom = romInit(0, 1);
    const uint8_t WRITTEN_DATA[] = {1, 2, 3, 4, 5, 6, 7, 8};
    ASSERT_EQ(rom.total_size, flashGetPageSize());

    // Wrong inputs
    ASSERT_EQ(romWrite(&rom, 0, NULL, 8), 0);
    ASSERT_EQ(romWrite(&rom, 4096, WRITTEN_DATA, 8), 0);
    ASSERT_EQ(romWrite(&rom, 0, WRITTEN_DATA, 0), 0);
    ASSERT_EQ(romWrite(&rom, 1500, WRITTEN_DATA, 1500), 0);
}

int main (int argc, char *argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
