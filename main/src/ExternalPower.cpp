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
// Created by jadjer on 01.10.23.
//

#include "ExternalPower.hpp"

#include <esp_sleep.h>

#include "gpio/InputPin.hpp"

auto constexpr EXTERNAL_POWER_ENABLED = gpio::PIN_LEVEL_HIGH;
auto constexpr EXTERNAL_POWER_DISABLED = gpio::PIN_LEVEL_LOW;

ExternalPower::ExternalPower(std::uint8_t const numberOfPin) : m_externalPowerPin(std::make_unique<gpio::InputPin>(numberOfPin, EXTERNAL_POWER_DISABLED)) {

  ESP_ERROR_CHECK(esp_sleep_enable_ext0_wakeup(static_cast<gpio_num_t>(numberOfPin), EXTERNAL_POWER_ENABLED));
}

bool ExternalPower::isEnabled() const {
  return (m_externalPowerPin->getLevel() == EXTERNAL_POWER_ENABLED);
}
