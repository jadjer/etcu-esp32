// Copyright 2023 Pavel Suprunov
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

//
// Created by jadjer on 28.09.23.
//

#include "ModeButton.hpp"

#include "gpio/Pin.hpp"

constexpr uint8_t numberOfModeButton1Pin = 7;
constexpr uint8_t numberOfModeButton2Pin = 6;

ModeButton::ModeButton() :
    m_modeButtonState(MODE_BUTTON_STATE_UNKNOWN),
    m_modeButton1(std::make_unique<gpio::Pin>(numberOfModeButton1Pin, gpio::PIN_LEVEL_HIGH)),
    m_modeButton2(std::make_unique<gpio::Pin>(numberOfModeButton2Pin, gpio::PIN_LEVEL_HIGH))
{
    assert(MODE_BUTTON_COUNT == 4);
}

void ModeButton::registerChangeValueCallback(ModeButtonChangeStateCallbackFunction const& changeStateCallbackFunction)
{
    m_changeStateCallbackFunction = changeStateCallbackFunction;
}

void ModeButton::process()
{
    auto mode1ButtonState = m_modeButton1->getLevel();
    auto mode2ButtonState = m_modeButton2->getLevel();

    ModeButtonState modeButtonState = MODE_BUTTON_STATE_UNKNOWN;

    if ((mode1ButtonState == gpio::PIN_LEVEL_HIGH) and (mode2ButtonState == gpio::PIN_LEVEL_LOW))
    {
        modeButtonState = MODE_BUTTON_STATE_MODE_1;
    }

    if ((mode1ButtonState == gpio::PIN_LEVEL_LOW) and (mode2ButtonState == gpio::PIN_LEVEL_LOW))
    {
        modeButtonState = MODE_BUTTON_STATE_MODE_2;
    }

    if ((mode1ButtonState == gpio::PIN_LEVEL_LOW) and (mode2ButtonState == gpio::PIN_LEVEL_HIGH))
    {
        modeButtonState = MODE_BUTTON_STATE_MODE_3;
    }

    if (modeButtonState == m_modeButtonState)
    {
        return;
    }

    m_modeButtonState = modeButtonState;

    if (m_changeStateCallbackFunction)
    {
        m_changeStateCallbackFunction(m_modeButtonState);
    }
}
