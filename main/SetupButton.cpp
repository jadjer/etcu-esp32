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

#include "gpio/Pin.hpp"

constexpr uint8_t numberOfSetupButtonPin = 5;

constexpr float threshold_InSeconds = 0.1;
constexpr float holdTimeDelay_InSeconds = 1.0;

SetupButton::SetupButton() :
    m_isHeld(false), m_isPressed(false), m_pressTime_InUS(esp_timer_get_time()), m_releaseTime_InUS(esp_timer_get_time()),
    m_setupButton(std::make_unique<gpio::Pin>(numberOfSetupButtonPin, gpio::PIN_LEVEL_HIGH))
{
    assert(SETUP_BUTTON_COUNT == 3);
}

void SetupButton::registerChangeValueCallback(SetupButtonChangeStateCallbackFunction const& changeStateCallbackFunction)
{
    m_changeStateCallbackFunction = changeStateCallbackFunction;
}

void SetupButton::process()
{
    auto buttonState = m_setupButton->getLevel();
    switch (buttonState)
    {
    case gpio::PIN_LEVEL_LOW:
        return processLowLevel();

    case gpio::PIN_LEVEL_HIGH:
        return processHighLevel();
    }
}

void SetupButton::processLowLevel()
{
    if (not m_isPressed)
    {
        return;
    }

    auto currentTime_InUS = esp_timer_get_time();

    m_releaseTime_InUS = currentTime_InUS;
    m_isHeld = false;
    m_isPressed = false;

    if (m_changeStateCallbackFunction)
    {
        m_changeStateCallbackFunction(SETUP_BUTTON_RELEASED);
    }
}

void SetupButton::processHighLevel()
{
    if (m_isHeld)
    {
        return;
    }

    auto currentTime_InUS = esp_timer_get_time();

    auto idleTime_InUS = currentTime_InUS - m_releaseTime_InUS;
    auto idleTime_InSeconds = static_cast<float>(idleTime_InUS) / 1000000;
    if (idleTime_InSeconds < threshold_InSeconds)
    {
        return;
    }

    if (m_isPressed)
    {
        return processHighLevelWhenPressed();
    }

    return processHighLevelWhenUnpressed();
}

void SetupButton::processHighLevelWhenPressed()
{
    auto currentTime_InUS = esp_timer_get_time();

    auto holdTime_InUS = currentTime_InUS - m_pressTime_InUS;
    auto holdTime_InSeconds = static_cast<float>(holdTime_InUS) / 1e6;
    if (holdTime_InSeconds > holdTimeDelay_InSeconds)
    {
        m_isHeld = true;

        if (m_changeStateCallbackFunction)
        {
            m_changeStateCallbackFunction(SETUP_BUTTON_HELD);
        }
    }
}

void SetupButton::processHighLevelWhenUnpressed()
{
    auto currentTime_InUS = esp_timer_get_time();

    m_pressTime_InUS = currentTime_InUS;
    m_isPressed = true;

    if (m_changeStateCallbackFunction)
    {
        m_changeStateCallbackFunction(SETUP_BUTTON_PRESSED);
    }
}
