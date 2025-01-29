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

#include "sensor/TwistPositionSensor.hpp"
#include "sensor/SensorBase.hpp"
#include <cstdint>

using Percent = std::uint8_t;

class TwistPosition : public SensorBase {
public:
  TwistPosition();
  ~TwistPosition() override = default;

public:
  [[nodiscard]] Percent getPercent() const;

private:
  TwistPositionSensor m_sensor1;
  TwistPositionSensor m_sensor2;
};
