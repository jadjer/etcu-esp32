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

#pragma once

#include <esp_task_wdt.h>

class ControllerBase {
public:
  virtual ~ControllerBase() = default;

public:
  [[noreturn]] void spin();

protected:
  virtual void spinOnce() = 0;

private:
  esp_task_wdt_user_handle_t m_watchdogHandle = nullptr;
};
