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

#include "indicator/Indicator.hpp"

#include "gpio/Pin.hpp"

namespace indicator
{

Indicator::Indicator(uint8_t pinNum) :
    m_enableFlag(true), m_taskValue(0), m_indicatorPin(std::make_unique<gpio::Pin>(pinNum, gpio::PIN_LEVEL_LOW))
{
}

Indicator::~Indicator()
{
    m_enableFlag = false;
    if (m_thread.joinable())
    {
        m_thread.join();
    }
}

void Indicator::enable()
{
    m_enableFlag = false;
    if (m_thread.joinable())
    {
        m_thread.join();
    }

    m_indicatorPin->setLevel(gpio::PIN_LEVEL_HIGH);
}

void Indicator::disable()
{
    m_enableFlag = false;
    if (m_thread.joinable())
    {
        m_thread.join();
    }

    m_indicatorPin->setLevel(gpio::PIN_LEVEL_LOW);
}

void Indicator::blink(int value)
{
    m_taskValue = value;

    if (m_enableFlag)
    {
        return;
    }

    m_enableFlag = true;
    m_thread = std::thread(&Indicator::blinkTask, this);
}

void Indicator::process() {}

} // namespace indicator
