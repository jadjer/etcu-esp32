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

Controller::Controller(ConfigurationPtr const &configuration) : m_configuration(configuration),
                                                                m_hondaECU(),
                                                                m_throttle(),
                                                                m_indicator(configuration->getIndicatorPin()),
                                                                m_modeSwitch(),
                                                                m_guardSwitch(),
                                                                m_breakSwitch(),
                                                                m_clutchSwitch(),
                                                                m_twistPosition() {
  if (m_guardSwitch.isEnabled()) {
    m_throttle.disable();
  }
}

bool Controller::hasError() const {
  if (m_throttle.hasError()) {
    return true;
  }

  if (m_twistPosition.hasError()) {
    return true;
  }

  if (m_hondaECU.hasError()) {
    return true;
  }

  return false;
}

void Controller::executeLogic() {
  if (m_throttle.isEnabled()) {
    auto const percentageOfTwisting = m_twistPosition.getPercent();
    m_throttle.setPosition(percentageOfTwisting);
  }

  if (hasError()) {
    m_indicator.setMode(ERROR_MODE);
  } else {
    m_indicator.setMode(NORMAL_MODE);
  }
}

void Controller::processComponents() {
  //  m_throttle.process();
  m_indicator.process();
}
