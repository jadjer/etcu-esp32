// Copyright 2022 Pavel Suprunov
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

//
// Created by jadjer on 26.09.22.
//

#include <indicator/ErrorCodeIndicator.hpp>

#include <driver/gpio.h>

ErrorCodeIndicator::ErrorCodeIndicator(int pinNum) : Indicator(pinNum) {}

ErrorCodeIndicator::~ErrorCodeIndicator() = default;

void ErrorCodeIndicator::enable() {}

void ErrorCodeIndicator::disable() {}

void ErrorCodeIndicator::blinkTask() {
    while (_enableFlag) {
        uint8_t firstDigit = _taskValue / 10;
        uint8_t secondDigit = _taskValue % 10;

        for (size_t i = 0; i < firstDigit; i++) {
            gpio_set_level(static_cast<gpio_num_t>(_pinNum), 1);
            std::this_thread::sleep_for(std::chrono::milliseconds(500));

            gpio_set_level(static_cast<gpio_num_t>(_pinNum), 0);
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        for (size_t i = 0; i < secondDigit; i++) {
            gpio_set_level(static_cast<gpio_num_t>(_pinNum), 1);
            std::this_thread::sleep_for(std::chrono::milliseconds(250));

            gpio_set_level(static_cast<gpio_num_t>(_pinNum), 0);
            std::this_thread::sleep_for(std::chrono::milliseconds(250));
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(3000));
    }
}
