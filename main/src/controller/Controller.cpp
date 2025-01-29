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

#include "controller/Controller.hpp"
#include <esp_log.h>

auto constexpr TAG = "Controller";

Controller::Controller(ConfigurationPtr configuration) : m_configuration(std::move(configuration)),
                                                         m_throttle(),
                                                         m_modeSwitch(),
                                                         m_guardSwitch(),
                                                         m_breakSwitch(),
                                                         m_clutchSwitch(),
                                                         m_indicator(),
                                                         m_twistPosition() {
}

void Controller::spinOnce() {
  auto const percentageOfTwisting = m_twistPosition.getPercent();

  ESP_LOGD(TAG, "Percentage of twisting position %d", percentageOfTwisting);

  m_throttle.setPercent(percentageOfTwisting);
}
