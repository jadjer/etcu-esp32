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

#include "SetupButton.hpp"

#include <esp_log.h>
#include <esp_timer.h>

#include "gpio/InputPin.hpp"

constexpr char const * tag = "setup_button";

SetupButton::SetupButton(uint8_t const numberOfSetupButtonPin, uint32_t const holdTimeInUS, uint32_t const thresholdInUS) :
    m_changeStateCallbackFunction(nullptr),
    m_setupButton(std::make_unique<gpio::InputPin>(numberOfSetupButtonPin, gpio::PIN_LEVEL_HIGH)),
    m_holdTime_InUS(holdTimeInUS),
    m_threshold_InUS(thresholdInUS),
    m_isHeld(false),
    m_isPressed(false),
    m_pressTime_InUS(0),
    m_releaseTime_InUS(0) {
}

void SetupButton::registerChangeValueCallback(SetupButtonChangeStateCallbackFunction const &changeStateCallbackFunction) {
  m_changeStateCallbackFunction = changeStateCallbackFunction;
}

void SetupButton::process() {
  if (not m_changeStateCallbackFunction) {
    return;
  }

  auto const buttonState = m_setupButton->getLevel();

  if (buttonState == gpio::PIN_LEVEL_HIGH) {
    return processButtonReleased();
  }

  if (buttonState == gpio::PIN_LEVEL_LOW) {
    return processButtonPressed();
  }
}

void SetupButton::processButtonReleased() {
  if (not m_changeStateCallbackFunction) {
    return;
  }

  if (not m_isPressed) {
    return;
  }

  m_releaseTime_InUS = esp_timer_get_time();
  m_isHeld = false;
  m_isPressed = false;

  m_changeStateCallbackFunction(SETUP_BUTTON_RELEASED);

  ESP_LOGI(tag, "Released");
}

void SetupButton::processButtonPressed() {
  if (not m_changeStateCallbackFunction) {
    return;
  }

  if (m_isHeld) {
    return;
  }

  auto const currentTime_InUS = esp_timer_get_time();

  auto const idleTime_InUS = currentTime_InUS - m_releaseTime_InUS;
  if (idleTime_InUS < m_threshold_InUS) {
    return;
  }

  if (m_isPressed) {
    auto const holdTime_InUS = currentTime_InUS - m_pressTime_InUS;
    if (holdTime_InUS > m_holdTime_InUS) {
      m_isHeld = true;

      m_changeStateCallbackFunction(SETUP_BUTTON_HELD);

      ESP_LOGI(tag, "Held");
    }
  }

  if (not m_isPressed) {
    m_isPressed = true;

    m_pressTime_InUS = currentTime_InUS;

    m_changeStateCallbackFunction(SETUP_BUTTON_PRESSED);

    ESP_LOGI(tag, "Pressed");
  }
}
