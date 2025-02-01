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

#include "ModeButton.hpp"

#include <esp_timer.h>

ModeButton::ModeButton(Pin pin, std::uint32_t holdTimeInUS, std::uint32_t thresholdInUS)
    : m_button(pin, gpio::PIN_LEVEL_HIGH),
      m_longPressedCallback(nullptr),
      m_shortPressedCallback(nullptr),
      m_holdTime_InUS(holdTimeInUS),
      m_threshold_InUS(thresholdInUS),
      m_isHeld(false),
      m_isPressed(false),
      m_pressTime_InUS(0),
      m_releaseTime_InUS(0) {
}

void ModeButton::registerLongPressedCallback(ModeButtonCallbackFunction const &callback) {
  m_longPressedCallback = callback;
}

void ModeButton::registerShortPressedCallback(ModeButtonCallbackFunction const &callback) {
  m_shortPressedCallback = callback;
}

void ModeButton::process() {
  auto const buttonState = m_button.getLevel();

  if (buttonState == gpio::PIN_LEVEL_HIGH) {
    return processButtonReleased();
  }

  if (buttonState == gpio::PIN_LEVEL_LOW) {
    return processButtonPressed();
  }
}

void ModeButton::processButtonReleased() {
  if (not m_isPressed) {
    return;
  }

  if (not m_isHeld) {
    if (m_shortPressedCallback) {
      m_shortPressedCallback();
    }
  }

  m_releaseTime_InUS = esp_timer_get_time();
  m_isHeld = false;
  m_isPressed = false;
}

void ModeButton::processButtonPressed() {
  if (m_isHeld) {
    return;
  }

  auto const currentTime_InUS = esp_timer_get_time();
  auto const idleTime_InUS = currentTime_InUS - m_releaseTime_InUS;
  if (idleTime_InUS < m_threshold_InUS) {
    return;
  }

  if (not m_isPressed) {
    m_isPressed = true;
    m_pressTime_InUS = currentTime_InUS;

    return;
  }

  auto const holdTime_InUS = currentTime_InUS - m_pressTime_InUS;
  if (holdTime_InUS < m_holdTime_InUS) {
    return;
  }

  m_isHeld = true;

  if (m_longPressedCallback) {
    m_longPressedCallback();
  }
}
