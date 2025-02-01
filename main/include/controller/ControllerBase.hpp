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
#include <esp_task_wdt.h>

class ControllerBase {
public:
  virtual ~ControllerBase() = default;

public:
  [[noreturn]] void loop();

protected:
  virtual void executeLogic() = 0;
  virtual void processComponents() = 0;

private:
  void watchdogTimerReset();

private:
  std::uint32_t m_watchdogResetLastTime = 0;
  esp_task_wdt_user_handle_t m_watchdogHandle = nullptr;
};
