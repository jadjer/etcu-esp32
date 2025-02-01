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

#include "controller/ControllerBase.hpp"

#include <esp_timer.h>

auto const MICRO_SECONDS_IN_SECOND = 1000000;
auto const WATCHDOG_TIMER_RESET_MICROSECONDS = 3 * MICRO_SECONDS_IN_SECOND;

[[noreturn]] void ControllerBase::loop() {
  ESP_ERROR_CHECK(esp_task_wdt_add_user("controller_loop", &m_watchdogHandle));

  while (true) {
    watchdogTimerReset();
    executeLogic();
    processComponents();
    vTaskDelay(1);
  }

  ESP_ERROR_CHECK(esp_task_wdt_delete_user(m_watchdogHandle));
}

void ControllerBase::watchdogTimerReset() {
  auto const currentTime = esp_timer_get_time();
  auto const timeDifference = currentTime - m_watchdogResetLastTime;
  if (timeDifference >= WATCHDOG_TIMER_RESET_MICROSECONDS) {
    ESP_ERROR_CHECK(esp_task_wdt_reset_user(m_watchdogHandle));
    m_watchdogResetLastTime = timeDifference;
  }
}
