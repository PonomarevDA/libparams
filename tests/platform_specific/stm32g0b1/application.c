/*
 * Copyright (c) 2022-2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "libparams/rom.h"
#include "libparams/flash_driver.h"

void test_flash_wr() {
    RomDriverInstance rom = romInit(255, 1);

    const uint8_t first_buf[2048];
    romWrite(&rom, 0, first_buf, flashGetPageSize());

    uint8_t second_buf[2048];
    romRead(&rom, 0, second_buf, flashGetPageSize());
}

int main() {
    test_flash_wr();

    return 0;
}
