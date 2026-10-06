/*
 * Copyright (c) 2026 Dmitry Ponomarev <ponomarevda96@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <stdio.h>
#include <string.h>
#include "storage.h"
#include "platform_flash_driver.h"
#include "params.h"

static int failures = 0;

static void check(int is_ok, const char* what) {
    if (!is_ok) {
        printf("FAIL: %s\n", what);
        failures++;
    }
}

static void init(void) {
    check(paramsInit(ubuntuFlashGetOps(), INTEGER_PARAMS_AMOUNT, NUM_OF_STR_PARAMS, -1, 1) == 0,
          "paramsInit");
    check(paramsLoad() == 0, "paramsLoad");
}

int main(void) {
    remove(LIBPARAMS_PARAMS_DIR "/nvm_params_0.yaml");
    ubuntuFlashSetLogLevel(LIBPARAMS_LOG_ERROR);

    init();
    check(paramsGetIntegerValue(PARAM_NODE_ID) == 42, "integer from the default file");
    check(paramsGetIntegerValue(PARAM_SYSTEM_VERSION) == 3, "immutable integer");
    check(strcmp((const char*)*paramsGetStringValue(PARAM_SYSTEM_NAME), "smoke") == 0, "string");
    check(paramsFind((const uint8_t*)"node.id", 7) == PARAM_NODE_ID, "paramsFind");

    paramsSetIntegerValue(PARAM_NODE_ID, 7);
    paramsSetIntegerValue(PARAM_SYSTEM_VERSION, 4);
    paramsSetStringValue(PARAM_SYSTEM_NAME, 5, (const uint8_t*)"saved");
    check(paramsSave() == 0, "paramsSave");

    init();
    check(paramsGetIntegerValue(PARAM_NODE_ID) == 7, "saved integer");
    check(paramsGetIntegerValue(PARAM_SYSTEM_VERSION) == 3, "immutable integer after save");
    check(strcmp((const char*)*paramsGetStringValue(PARAM_SYSTEM_NAME), "saved") == 0,
          "saved string");

    printf("%s\n", failures == 0 ? "smoke test passed" : "smoke test failed");
    return failures;
}
