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
// Created by jadjer on 01.10.23.
//

#include "Pump.hpp"

#include "gpio/OutputPin.hpp"

auto constexpr PUMP_ENABLED = gpio::PIN_LEVEL_HIGH;
auto constexpr PUMP_DISABLED = gpio::PIN_LEVEL_LOW;

Pump::Pump(std::uint8_t const numberOfPin) : m_pumpPin(std::make_unique<gpio::OutputPin>(numberOfPin, PUMP_DISABLED)) {
}

bool Pump::isEnabled() const {
  return (m_pumpPin->getLevel() == PUMP_ENABLED);
}

void Pump::enable() {
  if (isEnabled()) {
    return;
  }

  m_pumpPin->setLevel(PUMP_ENABLED);
}

void Pump::disable() {
  if (not isEnabled()) {
    return;
  }

  m_pumpPin->setLevel(PUMP_DISABLED);
}
