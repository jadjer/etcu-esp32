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

#include "TwistPositionSensor.hpp"

#include <executor/Node.hpp>
#include <cstdint>
#include <functional>

class TwistPosition : public executor::Node {
public:
  using Position = std::uint8_t;
  using ErrorCallback = std::function<void()>;
  using PositionChangeCallback = std::function<void(TwistPosition::Position)>;

public:
  TwistPosition();
  ~TwistPosition() override = default;

public:
  void registerErrorCallback(TwistPosition::ErrorCallback callback);
  void registerPositionChangeCallback(TwistPosition::PositionChangeCallback callback);

public:
  [[nodiscard]] TwistPosition::Position getPosition() const;

private:
  void process() override;

private:
  TwistPosition::ErrorCallback m_errorCallback = nullptr;
  TwistPosition::PositionChangeCallback m_positionChangeCallback = nullptr;

private:
  TwistPositionSensor m_sensor1;
  TwistPositionSensor m_sensor2;

private:
  TwistPosition::Position m_twistPosition = 0;
};
