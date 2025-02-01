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
// Created by jadjer on 9/20/24.
//

#pragma once

#include <nvs_handle.hpp>

#include "configuration/interface/Configuration.hpp"

class Configuration : public interface::Configuration {
public:
  Configuration();
  ~Configuration() override;

public:
  [[nodiscard]] interface::Pin getIndicatorPin() const override;
  [[nodiscard]] interface::Pin getModeButtonPin() const override;
  [[nodiscard]] interface::Pin getBreakSwitchPin() const override;
  [[nodiscard]] interface::Pin getGuardSwitchPin() const override;
  [[nodiscard]] interface::Pin getClutchSwitchPin() const override;
  [[nodiscard]] interface::Pin getTwistSensor1Pin() const override;
  [[nodiscard]] interface::Pin getTwistSensor2Pin() const override;

private:
  nvs_handle_t m_storageHandle;
};
