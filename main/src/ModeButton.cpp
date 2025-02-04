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
#include <utility>

ModeButton::ModeButton(ModeButton::Pin const pin, ModeButton::Time const holdTimeInUS, ModeButton::Time const thresholdInUS)
    : m_button(pin, gpio::PIN_LEVEL_HIGH),

      m_holdTime(holdTimeInUS),
      m_threshold(thresholdInUS),

      m_isHeld(false),
      m_isPressed(false),

      m_pressTime(0),
      m_releaseTime(0) {
}

void ModeButton::registerHoldCallback(ModeButton::HoldCallback callback) {
  m_holdCallback = std::move(callback);
}

void ModeButton::registerPressCallback(ModeButton::PressCallback callback) {
  m_pressCallback = std::move(callback);
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

void ModeButton::processButtonPressed() {
  if (m_isHeld) {
    return;
  }

  auto const currentTime = esp_timer_get_time();
  auto const idleTime = currentTime - m_releaseTime;
  if (idleTime < m_threshold) {
    return;
  }

  if (not m_isPressed) {
    m_isPressed = true;
    m_pressTime = currentTime;

    return;
  }

  auto const holdTime = currentTime - m_pressTime;
  if (holdTime < m_holdTime) {
    return;
  }

  m_isHeld = true;

  if (m_holdCallback) {
    m_holdCallback();
  }
}

void ModeButton::processButtonReleased() {
  if (not m_isPressed) {
    return;
  }

  if (not m_isHeld) {
    if (m_pressCallback) {
      m_pressCallback();
    }
  }

  m_releaseTime = esp_timer_get_time();
  m_isHeld = false;
  m_isPressed = false;
}
