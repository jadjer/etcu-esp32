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

#include "motor/Motor.hpp"

#include <esp_log.h>

auto const TAG = "Motor";

Motor::Motor() {
  m_encoder.setPowerMode(AS5600::PowerMode::POWER_MODE_NORMAL);
  m_encoder.setSlowFilter(AS5600::SlowFilter::SLOW_FILTER_X16);
  m_encoder.setFastFilterThreshold(AS5600::FastFilterThreshold::SLOW_FILTER_ONLY);
  m_encoder.setWatchdog(true);

  //  m_encoder.setCurrentPositionAsHome();
}

void Motor::process() {
  auto const angleRaw = m_encoder.getRawAngle();
  auto const angle = m_encoder.getAngle();
  ESP_LOGI(TAG, "Angle: %d, (%d)", angle, angleRaw);
}
