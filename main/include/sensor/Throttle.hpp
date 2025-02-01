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

#include "sensor/SensorBase.hpp"
#include <cstdint>

using Position = std::uint8_t;

class Throttle : public SensorBase {
public:
  Throttle();
  ~Throttle() override = default;

public:
  void setPosition(Position position) const;

public:
  void setMinimalPosition(Position position);
  void setMaximalPosition(Position position);

public:
  void enable();
  void disable();

public:
  [[nodiscard]] bool isEnabled() const;

private:
  void process() override;

private:
  bool m_enable;
  Position m_minimalPosition;
  Position m_maximalPosition;
};
