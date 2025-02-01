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
#include <gpio/OutputPin.hpp>
#include <vector>

using Pin = std::uint8_t;
using Interval = std::uint32_t;

enum IndicatorMode {
  DISABLE_MODE = 0,
  NORMAL_MODE,
  ERROR_MODE,
  CRUISE_READY_MODE,
  CRUISE_ON_MODE,
};

struct IntervalSetting {
  Interval interval;
  gpio::PinLevel indicatorLevel;
};

using Intervals = std::vector<IntervalSetting>;

class Indicator {
public:
  explicit Indicator(Pin pin);

public:
  void setMode(IndicatorMode mode);

public:
  void process();

private:
  gpio::OutputPin m_indicator;

private:
  IndicatorMode m_mode;
  Intervals m_intervals;
  std::size_t m_currentStep;
  std::uint32_t m_previousTime;
};
