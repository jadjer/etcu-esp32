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

#include "Timer.hpp"

#include <esp_timer.h>

Timer::Timer() {
  esp_timer_create_args_t const timerConfig = {
      .callback = &Timer::callback,
      .arg = this,
      .dispatch_method = ESP_TIMER_TASK,
      .name = "timer",
      .skip_unhandled_events = true,
  };

  ESP_ERROR_CHECK(esp_timer_create(&timerConfig, &m_timerHandle));
}

bool Timer::isEnabled() const {
  return esp_timer_is_active(m_timerHandle);
}

void Timer::start(std::uint32_t const delay, TimerCallback callback) {
  if (isEnabled()) {
    return;
  }

  m_callback = std::move(callback);

  ESP_ERROR_CHECK(esp_timer_start_once(m_timerHandle, delay));
}

void Timer::stop() {
  if (not isEnabled()) {
    return;
  }

  ESP_ERROR_CHECK(esp_timer_stop(m_timerHandle));
}

void Timer::callback(void *arg) {
  auto const timer = static_cast<Timer *>(arg);

  if (timer->m_callback) {
    timer->m_callback();
  }
}
