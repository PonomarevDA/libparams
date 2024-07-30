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

class SimpleLogger {
public:
    explicit SimpleLogger(const char* module_name): module(module_name) {}
    template <typename... Args>
    void info(Args... args) {
        std::stringstream ss;
        (ss << ... << args);
        std::cout << module << ": " << ss.str() << std::endl;
    }

    template <typename... Args>
    void error(Args... args) {
        std::stringstream ss;
        (ss << ... << args);
        std::cerr << module << ": " << ss.str() << std::endl;
    }

private:
    const char* module;
};

#endif  // LIBPARAM_SIMPLE_LOGGER_HPP_
