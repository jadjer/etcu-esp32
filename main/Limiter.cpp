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
// Created by jadjer on 3/23/24.
//

#include "Limiter.hpp"

#include "gpio/InputPin.hpp"

Limiter::Limiter() : m_homeLimitPin(std::make_unique<gpio::InputPin>(0, gpio::PIN_LEVEL_HIGH)) {

}

bool Limiter::isActive() const {
  return m_homeLimitPin->getLevel() == gpio::PinLevel::PIN_LEVEL_LOW;
}

