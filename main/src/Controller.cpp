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

#include "Controller.hpp"

#include <esp_log.h>

auto const TAG = "Controller";

Controller::Controller(ConfigurationPtr configuration) : m_configuration(std::move(configuration)) {
}

void Controller::registerCruiseEnableCallback(ControllerCallback const &callback) {
  m_enableCallback = callback;
}

void Controller::registerCruiseDisableCallback(ControllerCallback const &callback) {
  m_disableCallback = callback;
}

void Controller::registerChangeThrottlePositionCallback(ControllerChangePositionCallback const &callback) {
  m_callback = callback;
}

void Controller::setRPM(std::uint16_t rpm) {
  m_RPM = rpm;
}

void Controller::setSpeed(std::uint16_t speed) {
  m_speed = speed;
}

void Controller::setTwistPosition(std::uint8_t const position) {
  ESP_LOGI(TAG, "Twist position: %u", position);

  m_twistPosition = position;

  calculateThrottlePosition();
}

void Controller::modeButtonLongPressed() {
  ESP_LOGI(TAG, "Mode button held");

//  if (m_speed == 0) {
//        m_throttleMinimalPosition = m_throttlePosition
//  }
}

void Controller::modeButtonShortPressed() {
  ESP_LOGI(TAG, "Mode button pressed");
}

void Controller::enable() {
  m_throttleMaximalPosition = 100;
}

void Controller::disable() {
  m_throttleMaximalPosition = 0;
}

void Controller::calculateThrottlePosition() {
  if (m_callback) {
    m_callback(m_throttlePosition);
  }
}
