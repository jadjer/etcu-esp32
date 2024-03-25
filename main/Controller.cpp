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

#include "Controller.hpp"

Controller::Controller() : _clutchIsEnabled(true),
                           _accelerationValue(0),
                           _revolutionPerMinute(0),
                           _minimalAccelerationValue(0),
                           _speed_InKilometersPerHour(0),
                           _cruiseSpeed_InKilometersPerHour(0) {

}

Controller::~Controller() = default;

void Controller::registerChangeValueCallback(ControllerChangeValueCallbackFunction const &controllerChangeValueCallbackFunction) {
  _controllerChangeValueCallbackFunction = controllerChangeValueCallbackFunction;
}

void Controller::setRPM(uint32_t revolutionPerMinute) {
  _revolutionPerMinute = revolutionPerMinute;
}

void Controller::setSpeed(uint32_t speedInKilometersPerHour) {
  _speed_InKilometersPerHour = speedInKilometersPerHour;
}

void Controller::setClutch(bool clutchIsEnabled) {
  _clutchIsEnabled = clutchIsEnabled;
}

void Controller::setAcceleration(uint32_t accelerationValue) {
  _accelerationValue = accelerationValue;
}

void Controller::enable() {
  _minimalAccelerationValue = _accelerationValue;
  _cruiseSpeed_InKilometersPerHour = _speed_InKilometersPerHour;
}

void Controller::disable() {
  _minimalAccelerationValue = 0;
  _cruiseSpeed_InKilometersPerHour = 0;
}

void Controller::process() {
  auto value = _accelerationValue;
  if (value < _minimalAccelerationValue) {
    value = _minimalAccelerationValue;
  }

  if (_cruiseSpeed_InKilometersPerHour > 0) {
    if (_speed_InKilometersPerHour < _cruiseSpeed_InKilometersPerHour) {
      value += 10;
    }
    if (_speed_InKilometersPerHour > _cruiseSpeed_InKilometersPerHour) {
      value -= 10;
    }

    if (_revolutionPerMinute < 2500 or _revolutionPerMinute > 6000 or not _clutchIsEnabled) {
      _cruiseSpeed_InKilometersPerHour = 0;
    }
  }

  if (_revolutionPerMinute > 6000) {
    _minimalAccelerationValue = 0;
  }

  if (_controllerChangeValueCallbackFunction) {
    _controllerChangeValueCallbackFunction(value);
  }
}
