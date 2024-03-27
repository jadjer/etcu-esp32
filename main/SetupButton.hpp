// Copyright 2024 Pavel Suprunov
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
// Created by jadjer on 16.08.23.
//

#pragma once

#include <memory>
#include <functional>

#include "gpio/PinLevel.hpp"
#include "gpio/interface/IInputPin.hpp"
#include "executor/Node.hpp"

enum SetupButtonState {
  SETUP_BUTTON_RELEASED = 0,
  SETUP_BUTTON_PRESSED,
  SETUP_BUTTON_HELD
};

using PinLevel = gpio::PinLevel;
using SetupButtonChangeStateCallbackFunction = std::function<void(SetupButtonState)>;

class SetupButton : public executor::Node {
public:
  explicit SetupButton(uint8_t numberOfSetupButtonPin = 5, uint32_t holdTimeInUS = 1000000, uint32_t thresholdInUS = 100000);
  ~SetupButton() override = default;

public:
  void registerChangeValueCallback(SetupButtonChangeStateCallbackFunction const &changeStateCallbackFunction);

private:
  void process() override;

private:
  void processButtonReleased();
  void processButtonPressed();

private:
  SetupButtonChangeStateCallbackFunction m_changeStateCallbackFunction;

private:
  IInputPinPtr<PinLevel> m_setupButton;

private:
  uint32_t const m_holdTime_InUS;
  uint32_t const m_threshold_InUS;

private:
  bool m_isHeld;
  bool m_isPressed;

private:
  uint32_t m_pressTime_InUS;
  uint32_t m_releaseTime_InUS;
};
