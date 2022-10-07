/**
 * @file test_stm32f1xx.cpp
 * @author d.ponomarev
 * @date Aug 01, 2022
 */

#include <gtest/gtest.h>
#include <iostream>
#include "rom.h"

TEST(FlashHal, test_rom_init_ok) {
    auto res = romInit(0, 1);
    ASSERT_EQ(res, 0);
}

TEST(FlashHal, test_rom_write_read_ok) {
    const uint8_t WRITTEN_DATA[] = {1, 2, 3, 4, 5, 6, 7, 8};
    auto res = romInit(0, 1);
    ASSERT_EQ(res, 0);

    romBeginWrite();
    size_t write_res = romWrite(0, WRITTEN_DATA, 8);
    romEndWrite();
    ASSERT_EQ(write_res, 8);

    uint8_t read_data[8] = {};
    size_t read_res = romRead(0, read_data, 8);
    ASSERT_EQ(read_res, 8);
    int compare_res = memcmp(WRITTEN_DATA, read_data, 8);
    ASSERT_EQ(compare_res, 0);
}

int main (int argc, char *argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
