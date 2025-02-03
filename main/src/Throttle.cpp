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

#include "Throttle.hpp"

#include <esp_log.h>
#include <utility>

auto const TAG = "Throttle";
auto const THROTTLE_MINIMAL_POSITION = 0;
auto const THROTTLE_MAXIMAL_POSITION = 100;

Throttle::Throttle() : m_enable(true),
                       m_minimalPosition(THROTTLE_MINIMAL_POSITION),
                       m_maximalPosition(THROTTLE_MAXIMAL_POSITION) {
}

void Throttle::registerErrorCallback(Throttle::ErrorCallback callback) {
  m_errorCallback = std::move(callback);
}

void Throttle::registerPositionChangeCallback(Throttle::PositionChangeCallback callback) {
  m_positionChangeCallback = std::move(callback);
}

void Throttle::setPosition(Throttle::Position position) const {
  auto requiredPosition = position;

  if (requiredPosition < m_minimalPosition) {
    requiredPosition = m_minimalPosition;
    ESP_LOGW(TAG, "Limiting the throttle position to the minimum value of %d", m_minimalPosition);
  }

  if (requiredPosition > m_maximalPosition) {
    requiredPosition = m_maximalPosition;
    ESP_LOGW(TAG, "Limiting the throttle position to the maximum value of %d", m_maximalPosition);
  }

  //  m_throttle.setPercent(setPercentage);

  ESP_LOGI(TAG, "Throttle position is set to %d", requiredPosition);
}

void Throttle::setMinimalPosition(Throttle::Position position) {
  m_minimalPosition = position;

  if (m_minimalPosition < THROTTLE_MINIMAL_POSITION) {
    m_minimalPosition = THROTTLE_MINIMAL_POSITION;
  }

  if (m_minimalPosition > m_maximalPosition) {
    m_minimalPosition = m_maximalPosition;
  }
}

void Throttle::setMaximalPosition(Throttle::Position position) {
  m_maximalPosition = position;

  if (m_maximalPosition > THROTTLE_MAXIMAL_POSITION) {
    m_maximalPosition = THROTTLE_MAXIMAL_POSITION;
  }

  if (m_maximalPosition < m_minimalPosition) {
    m_maximalPosition = m_minimalPosition;
  }
}

void Throttle::enable() {
  m_enable = true;
}

void Throttle::disable() {
  m_enable = false;
}

bool Throttle::isEnabled() const {
  return m_enable;
}

void Throttle::process() {
}
