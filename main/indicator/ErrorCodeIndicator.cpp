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

#include "indicator/ErrorCodeIndicator.hpp"

#include "driver/gpio.h"

namespace indicator
{

constexpr uint32_t longLight_InMS = 500;
constexpr uint32_t shortLight_InMS = 250;
constexpr uint32_t delayBetweenDigits_InMS = 250;
constexpr uint32_t delayBetweenCodes_InMS = 3000;

ErrorCodeIndicator::ErrorCodeIndicator(int pinNum) : Indicator(pinNum) {}

ErrorCodeIndicator::~ErrorCodeIndicator() = default;

void ErrorCodeIndicator::enable() {}

void ErrorCodeIndicator::disable() {}

void ErrorCodeIndicator::blinkTask()
{
    while (m_enableFlag)
    {
        uint8_t firstDigit = m_taskValue / 10;
        uint8_t secondDigit = m_taskValue % 10;

        for (size_t i = 0; i < firstDigit; i++)
        {
            m_indicatorPin->setLevel(gpio::PIN_LEVEL_HIGH);
            std::this_thread::sleep_for(std::chrono::milliseconds(longLight_InMS));

            m_indicatorPin->setLevel(gpio::PIN_LEVEL_LOW);
            std::this_thread::sleep_for(std::chrono::milliseconds(longLight_InMS));
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(delayBetweenDigits_InMS));

        for (size_t i = 0; i < secondDigit; i++)
        {
            m_indicatorPin->setLevel(gpio::PIN_LEVEL_HIGH);
            std::this_thread::sleep_for(std::chrono::milliseconds(shortLight_InMS));

            m_indicatorPin->setLevel(gpio::PIN_LEVEL_LOW);
            std::this_thread::sleep_for(std::chrono::milliseconds(shortLight_InMS));
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(delayBetweenCodes_InMS));
    }
}

} // namespace indicator
