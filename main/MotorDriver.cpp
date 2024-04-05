// Copyright 2023 Pavel Suprunov
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

//
// Created by jadjer on 29.09.23.
//

#include "MotorDriver.hpp"

#include <thread>
#include <esp_timer.h>

#include "gpio/InputPin.hpp"
#include "gpio/OutputPin.hpp"

MotorDriver::MotorDriver() : m_stepPin(std::make_unique<gpio::OutputPin>(11)),
                             m_decayPin(std::make_unique<gpio::OutputPin>(14)),
                             m_mode0Pin(std::make_unique<gpio::OutputPin>(10)),
                             m_mode1Pin(std::make_unique<gpio::OutputPin>(9)),
                             m_mode2Pin(std::make_unique<gpio::OutputPin>(20)),
                             m_resetPin(std::make_unique<gpio::OutputPin>(48, gpio::PIN_LEVEL_HIGH)),
                             m_sleepPin(std::make_unique<gpio::OutputPin>(47, gpio::PIN_LEVEL_HIGH)),
                             m_enablePin(std::make_unique<gpio::OutputPin>(12)),
                             m_directionPin(std::make_unique<gpio::OutputPin>(13)),

                             m_inHomePin(std::make_unique<gpio::InputPin>(19, gpio::PIN_LEVEL_HIGH)),
                             m_isFaultPin(std::make_unique<gpio::InputPin>(21, gpio::PIN_LEVEL_HIGH)),

                             m_stepMinimalTime(2),
                             m_stepUpTime(0),
                             m_stepDownTime(0),

                             m_minimalPeriod(1 / 250000),
                             m_lastStepTime(0),

                             m_microstep(1) {
  MotorDriver::setDirection(motor::driver::MOTOR_ROTATE_CW);
  MotorDriver::setMicrostep(motor::driver::MOTOR_FULL_STEP);
}

uint32_t MotorDriver::getMicrostep() const {
  return m_microstep;
}

void MotorDriver::setDirection(int8_t direction) {
  if (direction == motor::driver::MOTOR_ROTATE_CW) {
    m_directionPin->setLevel(gpio::PIN_LEVEL_LOW);
  }

  if (direction == motor::driver::MOTOR_ROTATE_CCW) {
    m_directionPin->setLevel(gpio::PIN_LEVEL_HIGH);
  }
}

void MotorDriver::setMicrostep(uint32_t const microstep) {
  if (microstep >= 32) {
    m_microstep = 32;
    m_mode0Pin->setLevel(gpio::PIN_LEVEL_HIGH);
    m_mode1Pin->setLevel(gpio::PIN_LEVEL_LOW);
    m_mode2Pin->setLevel(gpio::PIN_LEVEL_HIGH);
    return;
  }

  if (microstep >= 16) {
    m_microstep = 16;
    m_mode0Pin->setLevel(gpio::PIN_LEVEL_LOW);
    m_mode1Pin->setLevel(gpio::PIN_LEVEL_LOW);
    m_mode2Pin->setLevel(gpio::PIN_LEVEL_HIGH);
    return;
  }

  if (microstep >= 8) {
    m_microstep = 8;
    m_mode0Pin->setLevel(gpio::PIN_LEVEL_HIGH);
    m_mode1Pin->setLevel(gpio::PIN_LEVEL_HIGH);
    m_mode2Pin->setLevel(gpio::PIN_LEVEL_LOW);
    return;
  }

  if (microstep >= 4) {
    m_microstep = 4;
    m_mode0Pin->setLevel(gpio::PIN_LEVEL_LOW);
    m_mode1Pin->setLevel(gpio::PIN_LEVEL_HIGH);
    m_mode2Pin->setLevel(gpio::PIN_LEVEL_LOW);
    return;
  }

  if (microstep >= 2) {
    m_microstep = 2;
    m_mode0Pin->setLevel(gpio::PIN_LEVEL_HIGH);
    m_mode1Pin->setLevel(gpio::PIN_LEVEL_LOW);
    m_mode2Pin->setLevel(gpio::PIN_LEVEL_LOW);
    return;
  }

  if (microstep >= 1) {
    m_microstep = 1;
    m_mode0Pin->setLevel(gpio::PIN_LEVEL_LOW);
    m_mode1Pin->setLevel(gpio::PIN_LEVEL_LOW);
    m_mode2Pin->setLevel(gpio::PIN_LEVEL_LOW);
    return;
  }
}

bool MotorDriver::isEnabled() const {
  return m_isFaultPin->getLevel() == gpio::PIN_LEVEL_LOW;
}

bool MotorDriver::isSleeping() const {
  return m_isFaultPin->getLevel() == gpio::PIN_LEVEL_LOW;
}

bool MotorDriver::inHome() const {
  return m_inHomePin->getLevel() == gpio::PIN_LEVEL_LOW;
}

bool MotorDriver::isFault() const {
  return m_isFaultPin->getLevel() == gpio::PIN_LEVEL_LOW;
}

void MotorDriver::enable() {
  m_enablePin->setLevel(gpio::PIN_LEVEL_LOW);
}

void MotorDriver::disable() {
  m_enablePin->setLevel(gpio::PIN_LEVEL_HIGH);
}

void MotorDriver::sleep() {
  m_sleepPin->setLevel(gpio::PIN_LEVEL_LOW);
}

void MotorDriver::wake() {
  m_sleepPin->setLevel(gpio::PIN_LEVEL_HIGH);
}

void MotorDriver::stepUp() {
  auto const currentTime = esp_timer_get_time();

  auto const timeBetweenSteps = currentTime - m_lastStepTime;
  auto const timeBetweenEdges = currentTime - m_stepDownTime;

  if (timeBetweenEdges < m_stepMinimalTime or timeBetweenSteps < m_minimalPeriod) {
    if (timeBetweenEdges > timeBetweenSteps) {
      std::this_thread::sleep_for(std::chrono::microseconds(timeBetweenEdges));
    } else {
      std::this_thread::sleep_for(std::chrono::microseconds(timeBetweenSteps));
    }
  }

  m_stepPin->setLevel(gpio::PIN_LEVEL_HIGH);
  m_stepUpTime = esp_timer_get_time();
}

void MotorDriver::stepDown() {
  auto const currentTime = esp_timer_get_time();

  auto const timeBetweenEdges = currentTime - m_stepUpTime;

  if (timeBetweenEdges < m_stepMinimalTime) {
    std::this_thread::sleep_for(std::chrono::microseconds(timeBetweenEdges));
  }

  m_stepPin->setLevel(gpio::PIN_LEVEL_LOW);
  m_stepDownTime = m_lastStepTime = esp_timer_get_time();
}
