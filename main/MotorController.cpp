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
// Created by jadjer on 3/24/24.
//

#include "MotorController.hpp"

#include "MotorDriver.hpp"

MotorController::MotorController() : m_motorController(std::make_unique<motor::MotorController>(std::make_unique<MotorDriver>())) {

}

void MotorController::process() {
  m_motorController->processMovement();
}

void MotorController::setSpeed(float speed) {
  m_motorController->setSpeedInStepsPerSecond(speed);
}

void MotorController::setAcceleration(float acceleration) {
  m_motorController->setAccelerationInStepsPerSecondPerSecond(acceleration);
}

void MotorController::setDeceleration(float deceleration) {
  m_motorController->setDecelerationInStepsPerSecondPerSecond(deceleration);
}

void MotorController::setPosition(int32_t position) {
  m_motorController->setTargetPositionInSteps(position);
}
