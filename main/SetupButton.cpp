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
// Created by jadjer on 16.08.23.
//

#include "SetupButton.hpp"

#include <esp_timer.h>

SetupButton::SetupButton(uint8_t pinNum, bool invertedValue) :
    m_isHeld(false), m_isPressed(false), m_pressTime_InUS(esp_timer_get_time()), m_releaseTime_InUS(esp_timer_get_time()),
    m_setupButton(pinNum, invertedValue)
{

    assert(SETUP_BUTTON_COUNT == 3);
}

void SetupButton::registerChangeValueCallback(SetupButtonChangeStateCallbackFunction const& changeStateCallbackFunction)
{
    m_changeStateCallbackFunction = changeStateCallbackFunction;
}

void SetupButton::process()
{
    auto buttonState = m_setupButton.isPressed();
    auto currentTime_InUS = esp_timer_get_time();

    if (buttonState == 0 and m_isPressed)
    {
        m_releaseTime_InUS = currentTime_InUS;
        m_isHeld = false;
        m_isPressed = false;

        if (m_changeStateCallbackFunction)
        {
            m_changeStateCallbackFunction(SETUP_BUTTON_RELEASED);
        }
    }

    if (buttonState == 1)
    {
        auto idleTime_InUS = currentTime_InUS - m_releaseTime_InUS;
        auto idleTime_InSeconds = static_cast<float>(idleTime_InUS) / 1e6;
        if (idleTime_InSeconds < 0.1)
        {
            return;
        }
    }

    if (buttonState == 1 and not m_isPressed and not m_isHeld)
    {
        m_pressTime_InUS = currentTime_InUS;
        m_isPressed = true;

        if (m_changeStateCallbackFunction)
        {
            m_changeStateCallbackFunction(SETUP_BUTTON_PRESSED);
        }
    }

    if (buttonState == 1 and m_isPressed and not m_isHeld)
    {
        auto holdTime_InUS = currentTime_InUS - m_pressTime_InUS;
        auto holdTime_InSeconds = static_cast<float>(holdTime_InUS) / 1e6;
        if (holdTime_InSeconds > 1)
        {
            m_isHeld = true;

            if (m_changeStateCallbackFunction)
            {
                m_changeStateCallbackFunction(SETUP_BUTTON_HELD);
            }
        }
    }
}
