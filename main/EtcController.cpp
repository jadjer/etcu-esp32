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
// Created by jadjer on 29.08.22.
//

#include "EtcController.hpp"

EtcController::EtcController() :
    m_changeValueCallbackFunction(nullptr),
    m_clutchIsEnabled(true),
    m_accelerationValue(0),
    m_revolutionPerMinute(0),
    m_minimalAccelerationValue(0),
    m_speed_InKilometersPerHour(0),
    m_cruiseSpeed_InKilometersPerHour(0) {

}

EtcController::~EtcController() = default;

void EtcController::registerChangeValueCallback(EtcControllerChangeValueCallbackFunction const &changeValueCallbackFunction) {
  m_changeValueCallbackFunction = changeValueCallbackFunction;
}

void EtcController::setRPM(uint32_t revolutionPerMinute) {
  m_revolutionPerMinute = revolutionPerMinute;
}

void EtcController::setSpeed(uint32_t speedInKilometersPerHour) {
  m_speed_InKilometersPerHour = speedInKilometersPerHour;
}

void EtcController::setClutch(bool clutchIsEnabled) {
  m_clutchIsEnabled = clutchIsEnabled;
}

void EtcController::setAcceleration(uint32_t accelerationValue) {
  m_accelerationValue = accelerationValue;
}

void EtcController::modeEnable() {
  m_minimalAccelerationValue = m_accelerationValue;
  m_cruiseSpeed_InKilometersPerHour = m_speed_InKilometersPerHour;
}

void EtcController::modeDisable() {
  m_minimalAccelerationValue = 0;
  m_cruiseSpeed_InKilometersPerHour = 0;
}

void EtcController::process() {
  auto acceleratorValue = m_accelerationValue;
  if (acceleratorValue < m_minimalAccelerationValue) {
    acceleratorValue = m_minimalAccelerationValue;
  }

  if (m_cruiseSpeed_InKilometersPerHour > 0) {
    if (m_speed_InKilometersPerHour < m_cruiseSpeed_InKilometersPerHour) {
      acceleratorValue += 10;
    }
    if (m_speed_InKilometersPerHour > m_cruiseSpeed_InKilometersPerHour) {
      acceleratorValue -= 10;
    }

    if (m_revolutionPerMinute < 2500 or m_revolutionPerMinute > 6000 or not m_clutchIsEnabled) {
      m_cruiseSpeed_InKilometersPerHour = 0;
    }
  }

  if (m_revolutionPerMinute > 6000) {
    m_minimalAccelerationValue = 0;
  }

  if (m_changeValueCallbackFunction) {
    m_changeValueCallbackFunction(acceleratorValue);
  }
}
