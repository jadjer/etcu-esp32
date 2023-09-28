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

ModeButton::ModeButton(uint8_t numberOfModeButton1Pin, uint8_t numberOfModeButton2Pin) :
    m_modeButton1(numberOfModeButton1Pin, true), m_modeButton2(numberOfModeButton2Pin, true),
    m_modeButtonState(MODE_BUTTON_STATE_UNKNOWN)
{
    assert(MODE_BUTTON_COUNT == 4);
}

void ModeButton::registerChangeValueCallback(ModeButtonChangeStateCallbackFunction const& changeStateCallbackFunction)
{
    m_changeStateCallbackFunction = changeStateCallbackFunction;
}

void ModeButton::process()
{
    auto mode1ButtonState = m_modeButton1.isPressed();
    auto mode2ButtonState = m_modeButton2.isPressed();

    ModeButtonState modeButtonState = MODE_BUTTON_STATE_UNKNOWN;

    if (mode1ButtonState and (not mode2ButtonState))
    {
        modeButtonState = MODE_BUTTON_STATE_MODE_1;
    }

    if ((not mode1ButtonState) and (not mode2ButtonState))
    {
        modeButtonState = MODE_BUTTON_STATE_MODE_2;
    }

    if ((not mode1ButtonState) and mode2ButtonState)
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
