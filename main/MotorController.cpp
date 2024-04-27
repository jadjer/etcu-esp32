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

#include <thread>
#include <esp_log.h>
#include <esp_timer.h>

#include "MotorDriver.hpp"

constexpr char const *tag = "motor_controller";

MotorController::MotorController(float const minSpeed, float const maxSpeed, uint32_t const maxSteps) :
    m_maxSteps(maxSteps),
    m_sleepAfterMotion_InUS(5 * 1000000),
    m_maxSpeed(maxSpeed),
    m_minSpeed(minSpeed),
    m_motorDriver(std::make_shared<MotorDriver>()),
    m_motorController(std::make_unique<motor::MotorController>(m_motorDriver)),
    m_speed(m_maxSpeed),
    m_lastMotionTime_InUS(0) {
  m_motorController->setMicrostep(32);
}

void MotorController::setSpeed(float const speed) {
  m_speed = speed;

  if (m_speed > m_maxSpeed) {
    m_speed = m_maxSpeed;
  }

  if (m_speed < m_minSpeed) {
    m_speed = m_minSpeed;
  }
}

void MotorController::setAcceleration(float const acceleration) {
  m_motorController->setAccelerationInStepsPerSecondPerSecond(acceleration);
}

void MotorController::setDeceleration(float const deceleration) {
  m_motorController->setDecelerationInStepsPerSecondPerSecond(deceleration);
}

void MotorController::setPosition(uint32_t const position) {
  if (not m_motorDriver->isEnabled()) {
    m_motorDriver->enable();
  }

  if (m_motorDriver->isSleeping()) {
    m_motorDriver->wake();
  }

  auto const position_InSteps = position * m_maxSteps / 100;
  auto const distanceToTargetSigned = m_motorController->getDistanceToTargetSigned();

  if (distanceToTargetSigned > 0) {
    m_motorController->setSpeedInStepsPerSecond(m_speed);
  }

  if (distanceToTargetSigned < 0) {
    m_motorController->setSpeedInStepsPerSecond(m_maxSpeed);
  }

  m_motorController->setTargetPositionInSteps(static_cast<int32_t>(position_InSteps));
}

void MotorController::moveToHome() {
  auto const stepsToHome = static_cast<int32_t>(-m_maxSteps);
  ESP_LOGI(tag, "Moving relative %ld steps", stepsToHome);

  m_motorDriver->enable();
  m_motorDriver->wake();
  m_motorController->setSpeedInStepsPerSecond(m_maxSpeed);
  m_motorController->moveToPositionInSteps(stepsToHome);
  m_motorController->setCurrentPositionAsHomeAndStop();
}

void MotorController::process() {
  if (not m_motorDriver->isEnabled()) {
    return;
  }

  if (m_motorDriver->isSleeping()) {
    return;
  }

  m_motorController->processMovement();

  if (not m_motorController->isHomed()) {
    m_lastMotionTime_InUS = esp_timer_get_time();
    return;
  }

  auto const currentTime_InUS = esp_timer_get_time();
  auto const timeWithoutMotion = currentTime_InUS - m_lastMotionTime_InUS;
  if (timeWithoutMotion >= m_sleepAfterMotion_InUS) {
    m_motorDriver->sleep();
  }
}
