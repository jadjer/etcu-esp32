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

#include "Controller.hpp"
#include "ModeButton.hpp"
#include "Switch.hpp"
#include "bluetooth/Bluetooth.hpp"
#include "configuration/interface/Configuration.hpp"
#include "Throttle.hpp"
#include "TwistPosition.hpp"

#include <executor/Executor.hpp>

class App {
public:
  using SwitchPtr = std::shared_ptr<Switch>;
  using ThrottlePtr = std::shared_ptr<Throttle>;
  using ControllerPtr = std::shared_ptr<Controller>;
  using ModeButtonPtr = std::shared_ptr<ModeButton>;
  using TwistPositionPtr = std::shared_ptr<TwistPosition>;

public:
  App();

public:
  void setup();
  void run();

private:
  ConfigurationPtr const m_configuration;

private:
  Bluetooth m_bluetooth;
  executor::Executor m_executor;

private:
  ThrottlePtr m_throttle = nullptr;
  SwitchPtr m_guardSwitch = nullptr;
  SwitchPtr m_breakSwitch = nullptr;
  SwitchPtr m_clutchSwitch = nullptr;
  ControllerPtr m_controller = nullptr;
  ModeButtonPtr m_modeButton = nullptr;
  TwistPositionPtr m_twistPosition = nullptr;
};
