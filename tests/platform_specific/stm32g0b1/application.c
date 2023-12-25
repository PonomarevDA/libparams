/*
 * Copyright (c) 2022-2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "rom.h"
#include "flash_driver.h"

void test_flash_wr() {
    romInit(255, 1);

    const uint8_t first_buf[PAGE_SIZE_BYTES];
    romWrite(0, first_buf, PAGE_SIZE_BYTES);

    uint8_t second_buf[PAGE_SIZE_BYTES];
    romRead(0, second_buf, PAGE_SIZE_BYTES);
}

int main() {
    test_flash_wr();

    return 0;
}
