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

#include <esp_log.h>

constexpr char const *tag = "etc_controller";

EtcController::EtcController() :
    m_changeMotorPositionCallbackFunction(nullptr),
    m_clutchIsEnabled(true),
    m_acceleratorCurrentValue(0),
    m_acceleratorMinimalValue(0),
    m_vehicleTPS_InMillivolts(0),
    m_vehicleSpeed_InKilometersPerHour(0),
    m_vehicleRevolutions_InRevolutionsPerMinute(0),
    m_cruiseSpeed_InKilometersPerHour(0) {
}

void EtcController::registerChangeValueCallback(EtcControllerChangeValueCallbackFunction const &changeMotorPositionCallbackFunction) {
  m_changeMotorPositionCallbackFunction = changeMotorPositionCallbackFunction;
}

void EtcController::setVehicleRPM(uint32_t revolutions) {
  m_vehicleRevolutions_InRevolutionsPerMinute = revolutions;
}

void EtcController::setVehicleSpeed(uint32_t speed) {
  m_vehicleSpeed_InKilometersPerHour = speed;
}

void EtcController::setVehicleClutchState(bool clutchIsEnabled) {
  m_clutchIsEnabled = clutchIsEnabled;
}

void EtcController::setAcceleratorValue(uint32_t acceleratorValue) {
  m_acceleratorCurrentValue = acceleratorValue;
}

void EtcController::modeEnable() {
  m_acceleratorMinimalValue = m_acceleratorCurrentValue;
  m_cruiseSpeed_InKilometersPerHour = m_vehicleSpeed_InKilometersPerHour;
}

void EtcController::modeDisable() {
  m_acceleratorMinimalValue = 0;
  m_cruiseSpeed_InKilometersPerHour = 0;
}

void EtcController::process() {
  if (not m_changeMotorPositionCallbackFunction) {
    return;
  }

  auto acceleratorValue = m_acceleratorCurrentValue;
  if (acceleratorValue < m_acceleratorMinimalValue) {
    acceleratorValue = m_acceleratorMinimalValue;
  }

  if (m_cruiseSpeed_InKilometersPerHour > 0) {
    if (m_vehicleSpeed_InKilometersPerHour < m_cruiseSpeed_InKilometersPerHour) {
      acceleratorValue += 10;
    }
    if (m_vehicleSpeed_InKilometersPerHour > m_cruiseSpeed_InKilometersPerHour) {
      acceleratorValue -= 10;
    }

    if (m_vehicleRevolutions_InRevolutionsPerMinute < 2500 or m_vehicleRevolutions_InRevolutionsPerMinute > 6000 or not m_clutchIsEnabled) {
      m_cruiseSpeed_InKilometersPerHour = 0;
    }
  }

  if (m_vehicleRevolutions_InRevolutionsPerMinute > 6000) {
    m_acceleratorMinimalValue = 0;
  }

  m_changeMotorPositionCallbackFunction(acceleratorValue);
}
