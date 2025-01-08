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
// Created by jadjer on 3/19/24.
//

#pragma once

#include <cstdint>

#include "gpio/PinLevel.hpp"
#include "gpio/interface/InputPin.hpp"

class WheelSensor {
  using PinLevel = gpio::PinLevel;
  using WheelSensorPin = InputPinPtr<PinLevel>;

public:
  WheelSensor(std::uint8_t numberOfPin, float wheelLength);

public:
  [[nodiscard]] float getDistance() const;
  [[nodiscard]] float getSpeed() const;

private:
  float m_wheelLength = 0;
  WheelSensorPin m_wheelSensorPin = nullptr;
};

#include <memory>

using WheelSensorPtr = std::unique_ptr<WheelSensor>;
