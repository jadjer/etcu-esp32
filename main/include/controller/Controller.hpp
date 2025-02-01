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

#include "HECU.hpp"
#include "Indicator.hpp"
#include "configuration/interface/Configuration.hpp"
#include "controller/ControllerBase.hpp"
#include "sensor/Throttle.hpp"
#include "sensor/TwistPosition.hpp"
#include "switch/BreakSwitch.hpp"
#include "switch/ClutchSwitch.hpp"
#include "switch/GuardSwitch.hpp"
#include "switch/ModeSwitch.hpp"
#include "switch/interface/Switch.hpp"

class Controller : public ControllerBase {
public:
  explicit Controller(ConfigurationPtr const &configuration);
  ~Controller() override = default;

public:
  [[nodiscard]] bool hasError() const;

private:
  void executeLogic() override;
  void processComponents() override;

protected:
  ConfigurationPtr m_configuration = nullptr;

protected:
  HECU m_hondaECU;
  Throttle m_throttle;
  Indicator m_indicator;
  ModeSwitch m_modeSwitch;
  GuardSwitch m_guardSwitch;
  BreakSwitch m_breakSwitch;
  ClutchSwitch m_clutchSwitch;
  TwistPosition m_twistPosition;
};
