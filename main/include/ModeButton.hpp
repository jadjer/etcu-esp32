// Copyright 2025 Pavel Suprunov
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

#pragma once

#include <executor/Node.hpp>
#include <functional>
#include <gpio/InputPin.hpp>

using Pin = std::uint8_t;
using ModeButtonCallbackFunction = std::function<void()>;

class ModeButton : public executor::Node {
public:
  explicit ModeButton(Pin pin, std::uint32_t holdTimeInUS = 1000000, std::uint32_t thresholdInUS = 100000);
  ~ModeButton() override = default;

public:
  void registerLongPressedCallback(ModeButtonCallbackFunction const &callback);
  void registerShortPressedCallback(ModeButtonCallbackFunction const &callback);

private:
  void process() override;
  void processButtonReleased();
  void processButtonPressed();

private:
  gpio::InputPin m_button;

private:
  ModeButtonCallbackFunction m_longPressedCallback;
  ModeButtonCallbackFunction m_shortPressedCallback;

private:
  std::uint32_t const m_holdTime_InUS;
  std::uint32_t const m_threshold_InUS;

private:
  bool m_isHeld;
  bool m_isPressed;

private:
  std::uint32_t m_pressTime_InUS;
  std::uint32_t m_releaseTime_InUS;
};
