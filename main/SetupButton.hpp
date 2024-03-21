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

#include <memory>
#include <functional>

#include "gpio/interface/IPin.hpp"
#include "executor/Node.hpp"

enum SetupButtonState
{
    SETUP_BUTTON_RELEASED = 0,
    SETUP_BUTTON_PRESSED,
    SETUP_BUTTON_HELD,
    SETUP_BUTTON_COUNT = 3
};

using SetupButtonChangeStateCallbackFunction = std::function<void(SetupButtonState)>;

class SetupButton : public executor::Node
{
public:
    SetupButton();
    ~SetupButton() override = default;

public:
    void registerChangeValueCallback(SetupButtonChangeStateCallbackFunction const& changeStateCallbackFunction);

private:
    void process() override;

private:
    void processLowLevel();
    void processHighLevel();
    void processHighLevelWhenPressed();
    void processHighLevelWhenUnpressed();

private:
    SetupButtonChangeStateCallbackFunction m_changeStateCallbackFunction = nullptr;

private:
    bool m_isHeld;
    bool m_isPressed;
    int64_t m_pressTime_InUS;
    int64_t m_releaseTime_InUS;
    std::unique_ptr<gpio::interface::IPin> m_setupButton;
};
