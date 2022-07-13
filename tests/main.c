/**
 * @file main.c
 * @author d.ponomarev
 * @date Jul 11, 2022
 */

#include "flash.h"

int main() {
    flashInit(127, 1);

    const uint8_t first_buf[PAGE_SIZE_BYTES];
    flashWrite(0, first_buf, PAGE_SIZE_BYTES);

    uint8_t second_buf[PAGE_SIZE_BYTES];
    flashRead(0, second_buf, PAGE_SIZE_BYTES);

    return 0;
}
