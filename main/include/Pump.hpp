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

#pragma once

#include <cstdint>

#include "gpio/PinLevel.hpp"
#include "gpio/interface/OutputPin.hpp"

class Pump {
  using PinLevel = gpio::PinLevel;
  using PumpPin = OutputPinPtr<PinLevel>;

public:
  explicit Pump(std::uint8_t numberOfPin);

public:
  [[nodiscard]] bool isEnabled() const;

public:
  void enable();
  void disable();

private:
  PumpPin m_pumpPin = nullptr;
};

#include <memory>

using PumpPtr = std::unique_ptr<Pump>;
