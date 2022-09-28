/**
 * @file test_stm32f1xx.cpp
 * @author d.ponomarev
 * @date Aug 01, 2022
 */

#include <gtest/gtest.h>
#include <iostream>
#include "main.h"

constexpr size_t FLASH_ADDR = 0x08000000;
constexpr size_t PAGE_SIZE = 1024;
constexpr size_t PAGE_IDX = 127;
constexpr size_t PAGES_AMOUNT = 1;
constexpr size_t ADDR_BEGIN = FLASH_ADDR + PAGE_SIZE * PAGE_IDX;
constexpr size_t ADDR_END = FLASH_ADDR + PAGE_SIZE * (PAGE_IDX + 1);

TEST(FlashHal, erase_ok) {
    HAL_FLASH_Init(ADDR_BEGIN);

    FLASH_EraseInitTypeDef FLASH_EraseInitStruct = {
        .TypeErase = FLASH_TYPEERASE_PAGES,
        .Banks = 0,
        .PageAddress = (uint32_t)ADDR_BEGIN,
        .NbPages = PAGES_AMOUNT
    };
    uint32_t error = 0;
    HAL_StatusTypeDef res = HAL_FLASHEx_Erase(&FLASH_EraseInitStruct, &error);
    ASSERT_EQ(res, HAL_OK);
}

TEST(FlashHal, erase_before) {
    HAL_FLASH_Init(ADDR_BEGIN);

    FLASH_EraseInitTypeDef FLASH_EraseInitStruct = {
        .TypeErase = FLASH_TYPEERASE_PAGES,
        .Banks = 0,
        .PageAddress = (uint32_t)(ADDR_BEGIN - 1),
        .NbPages = PAGES_AMOUNT
    };
    uint32_t error = 0;
    HAL_StatusTypeDef res = HAL_FLASHEx_Erase(&FLASH_EraseInitStruct, &error);
    ASSERT_EQ(res, HAL_ERROR);
}

TEST(FlashHal, erase_after) {
    HAL_FLASH_Init(ADDR_BEGIN);

    FLASH_EraseInitTypeDef FLASH_EraseInitStruct = {
        .TypeErase = FLASH_TYPEERASE_PAGES,
        .Banks = 0,
        .PageAddress = (uint32_t)(ADDR_BEGIN + 1),
        .NbPages = PAGES_AMOUNT
    };
    uint32_t error = 0;
    HAL_StatusTypeDef res = HAL_FLASHEx_Erase(&FLASH_EraseInitStruct, &error);
    ASSERT_EQ(res, HAL_ERROR);
}

TEST(FlashHal, program_first) {
    HAL_FLASH_Init(ADDR_BEGIN);

    HAL_StatusTypeDef res = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, ADDR_BEGIN, 42);
    ASSERT_EQ(res, HAL_OK);
}

TEST(FlashHal, program_last) {
    HAL_FLASH_Init(ADDR_BEGIN);

    HAL_StatusTypeDef res = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, ADDR_BEGIN + PAGE_SIZE - 4, 42);
    ASSERT_EQ(res, HAL_OK);
}

TEST(FlashHal, program_before) {
    HAL_FLASH_Init(ADDR_BEGIN);

    HAL_StatusTypeDef res = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, ADDR_BEGIN - 4, 42);
    ASSERT_EQ(res, HAL_ERROR);
}

TEST(FlashHal, program_after) {
    HAL_FLASH_Init(ADDR_BEGIN);

    HAL_StatusTypeDef res = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, ADDR_BEGIN + PAGE_SIZE, 42);
    ASSERT_EQ(res, HAL_ERROR);
}

int main (int argc, char *argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
