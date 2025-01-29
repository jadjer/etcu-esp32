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

#include "Indicator.hpp"
#include "Switch.hpp"
#include "configuration/interface/Configuration.hpp"
#include "controller/ControllerBase.hpp"
#include "sensor/Throttle.hpp"
#include "sensor/TwistPosition.hpp"

class Controller : public ControllerBase {
public:
  explicit Controller(ConfigurationPtr configuration);
  ~Controller() override = default;

private:
  void spinOnce() override;

private:
  ConfigurationPtr m_configuration = nullptr;

private:
  Throttle m_throttle;
  Switch m_modeSwitch;
  Switch m_guardSwitch;
  Switch m_breakSwitch;
  Switch m_clutchSwitch;
  Indicator m_indicator;
  TwistPosition m_twistPosition;
};
