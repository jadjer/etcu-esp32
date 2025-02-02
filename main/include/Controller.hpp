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

#pragma once

#include "configuration/interface/Configuration.hpp"
#include <functional>

using ControllerCallback = std::function<void()>;
using ControllerChangePositionCallback = std::function<void(std::uint8_t)>;

class Controller {
public:
  explicit Controller(ConfigurationPtr configuration);

public:
  void registerCruiseEnableCallback(ControllerCallback const& callback);
  void registerCruiseDisableCallback(ControllerCallback const& callback);
  void registerChangeThrottlePositionCallback(ControllerChangePositionCallback const &callback);

public:
  void setRPM(std::uint16_t rpm);
  void setSpeed(std::uint16_t speed);
  void setTwistPosition(std::uint8_t position);

public:
  void modeButtonLongPressed();
  void modeButtonShortPressed();

public:
  void enable();
  void disable();

private:
  void calculateThrottlePosition();

private:
  ConfigurationPtr m_configuration = nullptr;
  ControllerCallback m_enableCallback = nullptr;
  ControllerCallback m_disableCallback = nullptr;
  ControllerChangePositionCallback m_callback = nullptr;

private:
  std::uint8_t m_twistPosition = 0;
  std::uint8_t m_throttlePosition = 0;

private:
  std::uint16_t m_RPM = 0;
  std::uint16_t m_speed = 0;
  std::uint16_t m_throttleMinimalPosition = 0;
  std::uint16_t m_throttleMaximalPosition = 100;
};
