/*
 * Copyright (c) 2024 Dmitry Ponomarev <ponomarevda96@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef LIBPARAMS__LEGACY_FUNCTIONS_H_
#define LIBPARAMS__LEGACY_FUNCTIONS_H_

#define DO_PRAGMA(x) _Pragma(#x)
#define LEGACY_USAGE_NOTE DO_PRAGMA(message ("This function is legacy. It will be deprecated."))


// These function will be deprecated soon:
#define paramsLoadToFlash() (\
{\
    int8_t retval;\
    LEGACY_USAGE_NOTE;\
    retval = paramsSave();\
    retval;\
})

#define paramsLoadFromFlash() (\
{\
    int8_t retval;\
    LEGACY_USAGE_NOTE;\
    retval = paramsLoad();\
    retval;\
})

#define paramsGetParamName(param_idx) (\
{\
    const char* retval;\
    LEGACY_USAGE_NOTE;\
    retval = paramsGetName(param_idx);\
    retval;\
})

#define paramsGetIndexByName(name, len) (\
{\
    ParamIndex_t retval;\
    LEGACY_USAGE_NOTE;\
    retval = paramsFind(name, len);\
    retval;\
})

#endif  // LIBPARAMS__LEGACY_FUNCTIONS_H_
