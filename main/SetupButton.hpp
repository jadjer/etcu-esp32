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

#pragma once

#include <functional>
#include <button/Button.hpp>

enum SetupButtonState {
    SETUP_BUTTON_RELEASED = 0,
    SETUP_BUTTON_PRESSED,
    SETUP_BUTTON_HELD,
    SETUP_BUTTON_COUNT = 3
};

using SetupButtonChangeStateCallbackFunction = std::function<void(SetupButtonState)>;

class SetupButton : public Button {
public:
    explicit SetupButton(uint8_t pinNum, bool invertedValue = false);
    ~SetupButton() override;

public:
    void registerChangeValueCallback(SetupButtonChangeStateCallbackFunction const& setupButtonChangeStateCallbackFunction);

public:
    void process();

private:
    SetupButtonChangeStateCallbackFunction _setupButtonChangeStateCallbackFunction = nullptr;

private:
    bool _isHeld;
    bool _isPressed;
    int64_t _pressTime_InUS;
    int64_t _releaseTime_InUS;
};
