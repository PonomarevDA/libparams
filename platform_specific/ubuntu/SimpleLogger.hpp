/*
 * Copyright (c) 2024 Dmitry Ponomarev <ponomarevda96@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef LIBPARAM_SIMPLE_LOGGER_HPP_
#define LIBPARAM_SIMPLE_LOGGER_HPP_

#include <iostream>
#include <sstream>
#include "platform_flash_driver.h"

class SimpleLogger {
public:
    explicit SimpleLogger(const char* module_name): module(module_name) {}

    template <typename... Args>
    void debug(Args... args) {
        log(LIBPARAMS_LOG_DEBUG, args...);
    }

    template <typename... Args>
    void info(Args... args) {
        log(LIBPARAMS_LOG_INFO, args...);
    }

    template <typename... Args>
    void error(Args... args) {
        log(LIBPARAMS_LOG_ERROR, args...);
    }

    // Shared by all instances: the logger is a header-level static, one copy per translation unit.
    static inline LibparamsLogLevel level = static_cast<LibparamsLogLevel>(LIBPARAMS_LOG_LEVEL);
    static inline LibparamsLogHandler handler = nullptr;

private:
    template <typename... Args>
    void log(LibparamsLogLevel msg_level, Args... args) {
        if (msg_level > level) {
            return;
        }
        std::stringstream ss;
        (ss << ... << args);
        if (handler != nullptr) {
            handler(msg_level, ss.str().c_str());
        } else if (msg_level == LIBPARAMS_LOG_ERROR) {
            std::cerr << "[" << module << "]: [ERROR]: " << ss.str() << std::endl;
        } else if (msg_level == LIBPARAMS_LOG_INFO) {
            std::cout << "[" << module << "] [INFO]: " << ss.str() << std::endl;
        } else {
            std::cout << "[" << module << "] [DEBUG]: " << ss.str() << std::endl;
        }
    }

    const char* module;
};

#endif  // LIBPARAM_SIMPLE_LOGGER_HPP_
