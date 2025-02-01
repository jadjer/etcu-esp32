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

#include "sensor/TwistPosition.hpp"

TwistPosition::TwistPosition() : m_callback(nullptr),

                                 m_sensor1(),
                                 m_sensor2() {

  auto const sensorVoltage1 = m_sensor1.getVoltage();
  auto const sensorVoltage2 = m_sensor2.getVoltage();

  auto const voltageDifferenceFromSensors = sensorVoltage2 - sensorVoltage1;
  if (voltageDifferenceFromSensors) {
  }
}

void TwistPosition::registerPositionChangedCallback(TwistPositionChangePositionCallbackFunction const &callback) {
  m_callback = callback;
}

Position TwistPosition::getPosition() const {
  auto const sensorVoltage1 = m_sensor1.getVoltage();
  auto const sensorVoltage2 = m_sensor2.getVoltage();

  return 0;
}

void TwistPosition::process() {
  if (m_callback) {
    m_callback(123);
  }
}
