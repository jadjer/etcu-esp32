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

//
// Created by jadjer on 9/24/24.
//

#include "controller/ControllerBase.hpp"

auto constexpr TASK_RESET_PERIOD_MICROSECOND = 3000;

[[noreturn]] void ControllerBase::spin() {
  ESP_ERROR_CHECK(esp_task_wdt_add_user("controller_spin", &m_watchdogHandle));

  while (true) {
    ESP_ERROR_CHECK(esp_task_wdt_reset_user(m_watchdogHandle));
    spinOnce();
    vTaskDelay(pdMS_TO_TICKS(TASK_RESET_PERIOD_MICROSECOND));
  }

  ESP_ERROR_CHECK(esp_task_wdt_delete_user(m_watchdogHandle));
}
