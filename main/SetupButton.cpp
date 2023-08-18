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
        Button(pinNum, invertedValue),
        _isHeld(false),
        _isPressed(false),
        _pressTime_InUS(esp_timer_get_time()),
        _releaseTime_InUS(esp_timer_get_time()) {

    assert(SETUP_BUTTON_COUNT == 3);
}

SetupButton::~SetupButton() = default;

void SetupButton::registerChangeValueCallback(SetupButtonChangeStateCallbackFunction const &setupButtonChangeStateCallbackFunction) {
    _setupButtonChangeStateCallbackFunction = setupButtonChangeStateCallbackFunction;
}

void SetupButton::process() {
    auto buttonState = Button::isPressed();
    auto currentTime_InUS = esp_timer_get_time();

    if (buttonState == 0 and _isPressed) {
        _releaseTime_InUS = currentTime_InUS;
        _isHeld = false;
        _isPressed = false;

        if (_setupButtonChangeStateCallbackFunction) {
            _setupButtonChangeStateCallbackFunction(SETUP_BUTTON_RELEASED);
        }
    }

    if (buttonState == 1) {
        auto idleTime_InUS = currentTime_InUS - _releaseTime_InUS;
        auto idleTime_InSeconds = static_cast<float>(idleTime_InUS) / 1e6;
        if (idleTime_InSeconds < 0.1) {
            return;
        }
    }

    if (buttonState == 1 and not _isPressed and not _isHeld) {
        _pressTime_InUS = currentTime_InUS;
        _isPressed = true;

        if (_setupButtonChangeStateCallbackFunction) {
            _setupButtonChangeStateCallbackFunction(SETUP_BUTTON_PRESSED);
        }
    }

    if (buttonState == 1 and _isPressed and not _isHeld) {
        auto holdTime_InUS = currentTime_InUS - _pressTime_InUS;
        auto holdTime_InSeconds = static_cast<float>(holdTime_InUS) / 1e6;
        if (holdTime_InSeconds > 1) {
            _isHeld = true;

            if (_setupButtonChangeStateCallbackFunction) {
                _setupButtonChangeStateCallbackFunction(SETUP_BUTTON_HELD);
            }
        }
    }
}
