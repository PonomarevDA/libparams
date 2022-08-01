/**
 * @file main.c
 * @author d.ponomarev
 * @date Jul 11, 2022
 */

#include "flash.h"

void test_flash_wr() {
    flashInit(127, 1);

    const uint8_t first_buf[PAGE_SIZE_BYTES];
    flashWrite(0, first_buf, PAGE_SIZE_BYTES);

    uint8_t second_buf[PAGE_SIZE_BYTES];
    flashRead(0, second_buf, PAGE_SIZE_BYTES);
}

int main() {
    test_flash_wr();

    return 0;
}
