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

#pragma once

#include <cstdint>
#include <functional>

#include <esp_timer.h>

using TimerCallback = std::function<void()>;

class Timer {
public:
  Timer();

public:
  [[nodiscard]] bool isEnabled() const;

public:
  void start(std::uint32_t delay, TimerCallback callback);
  void stop();

private:
  static void callback(void *arg);

private:
  TimerCallback m_callback = nullptr;
  esp_timer_handle_t m_timerHandle = nullptr;
};

#include <memory>

using TimerPtr = std::unique_ptr<Timer>;
