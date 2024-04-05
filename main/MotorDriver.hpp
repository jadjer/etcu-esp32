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
// Created by jadjer on 29.09.23.
//

#pragma once

#include <memory>

#include "gpio/PinLevel.hpp"
#include "motor/driver/interface/IDriver.hpp"
#include "gpio/interface/IInputPin.hpp"
#include "gpio/interface/IOutputPin.hpp"

using PinLevel = gpio::PinLevel;
using PinInput = IInputPinPtr<PinLevel>;
using PinOutput = IOutputPinPtr<PinLevel>;

class MotorDriver : public motor::driver::interface::IDriver {
public:
  MotorDriver();
  ~MotorDriver() override = default;

public:
  [[nodiscard]] uint32_t getMicrostep() const override;

public:
  void setDirection(int8_t direction) override;
  void setMicrostep(uint32_t microstep) override;

public:
  [[nodiscard]] bool isEnabled() const override;
  [[nodiscard]] bool isSleeping() const override;
  [[nodiscard]] bool inHome() const override;
  [[nodiscard]] bool isFault() const override;

public:
  void enable() override;
  void disable() override;

public:
  void sleep() override;
  void wake() override;

public:
  void stepUp() override;
  void stepDown() override;

private:
  PinOutput m_stepPin;
  PinOutput m_decayPin;
  PinOutput m_mode0Pin;
  PinOutput m_mode1Pin;
  PinOutput m_mode2Pin;
  PinOutput m_resetPin;
  PinOutput m_sleepPin;
  PinOutput m_enablePin;
  PinOutput m_directionPin;

private:
  PinInput m_inHomePin;
  PinInput m_isFaultPin;

private:
  uint32_t const m_stepMinimalTime;
  uint32_t m_stepUpTime;
  uint32_t m_stepDownTime;

private:
  uint32_t const m_minimalPeriod;
  uint32_t m_lastStepTime;

private:
  uint32_t m_microstep;
};

#include <memory>

using MotorDriverPtr = std::unique_ptr<MotorDriver>;
