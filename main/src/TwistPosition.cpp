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

#include "TwistPosition.hpp"

#include <esp_log.h>
#include <utility>

auto const TAG = "Twist Position";

TwistPosition::TwistPosition() : m_adc(0),
                                 m_sensor1(m_adc.createChannel(3)),
                                 m_sensor2(m_adc.createChannel(6)) {
}

void TwistPosition::registerErrorCallback(TwistPosition::ErrorCallback callback) {
  m_errorCallback = std::move(callback);
}

void TwistPosition::registerPositionChangeCallback(TwistPosition::PositionChangeCallback callback) {
  m_positionChangeCallback = std::move(callback);
}

TwistPosition::Position TwistPosition::getPosition() const {
  return m_position;
}

void TwistPosition::process() {
  auto const voltageFromSensor1 = m_sensor1->getVoltage();
  auto const voltageFromSensor2 = m_sensor2->getVoltage();

  ESP_LOGI(TAG, "Voltage 1: %d, Voltage 2: %d", voltageFromSensor1, voltageFromSensor2);

  if (m_positionChangeCallback) {
    m_positionChangeCallback(m_position);
  }
}
