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

#include "WheelSensor.hpp"

#include "gpio/InputPin.hpp"

auto constexpr MICROSECONDS_PER_SECOND = 1000000;

WheelSensor::WheelSensor(std::uint8_t const numberOfPin, float const wheelLength) : m_wheelLength(wheelLength),
                                                                                    m_wheelSensorPin(std::make_unique<gpio::InputPin>(numberOfPin, PinLevel::PIN_LEVEL_LOW)) {
}

float WheelSensor::getDistance() const {
  auto const wheelCount = m_wheelSensorPin->getCount();
  auto const distance = m_wheelLength * static_cast<float>(wheelCount);

  return distance;
}

float WheelSensor::getSpeed() const {
  auto const delay_InMicroseconds = m_wheelSensorPin->getDelay();
  auto const delay_InSeconds = static_cast<float>(delay_InMicroseconds) / MICROSECONDS_PER_SECOND;
  auto const speed = m_wheelLength / delay_InSeconds;

  return speed;
}
