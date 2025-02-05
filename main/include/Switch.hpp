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

#include <cstdint>
#include <executor/Node.hpp>
#include <functional>
#include <gpio/InputPin.hpp>

class Switch : public executor::Node {
public:
  using Pin = std::uint8_t;
  using SwitchCallback = std::function<void(bool)>;

public:
  explicit Switch(Switch::Pin pin);
  ~Switch() override = default;

public:
  void registerSwitchCallback(Switch::SwitchCallback callback);

public:
  [[nodiscard]] virtual bool isEnabled() const;

private:
  void process() override;

private:
  Switch::SwitchCallback m_switchCallback = nullptr;

private:
  gpio::InputPin m_switch;

private:
  bool m_enable = false;
};
