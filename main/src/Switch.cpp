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

#include "Switch.hpp"

#include <utility>

Switch::Switch(Switch::Pin pin) : m_switch(pin, gpio::PIN_LEVEL_LOW) {
}

void Switch::registerSwitchCallback(Switch::SwitchCallback callback) {
  m_switchCallback = std::move(callback);
}

bool Switch::isEnabled() const {
  return m_enable;
}

void Switch::process() {
  auto const level = m_switch.getLevel();

  if (level == gpio::PIN_LEVEL_LOW) {

    if (m_enable) {
      m_enable = false;

      if (m_switchCallback) {
        m_switchCallback(m_enable);
      }
    }
  }

  if (level == gpio::PIN_LEVEL_HIGH) {

    if (not m_enable) {
      m_enable = true;

      if (m_switchCallback) {
        m_switchCallback(m_enable);
      }
    }
  }
}
