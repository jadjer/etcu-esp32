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

#include "motor/AS5600.hpp"

#include <executor/Node.hpp>

class Motor : public executor::Node {
public:
  using Position = std::uint16_t;

public:
  Motor();
  ~Motor() override = default;

public:
  void setPosition(Position position);

private:
  void process() override;

private:
  AS5600 m_encoder;

private:
  Motor::Position m_targetPosition = 0;
  Motor::Position m_currentPosition = 0;
};
