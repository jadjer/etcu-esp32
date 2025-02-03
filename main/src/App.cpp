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

#include "App.hpp"

#include "configuration/Configuration.hpp"

auto const TAG = "APP";

auto speed = 80;

App::App() : m_configuration(std::make_shared<Configuration>()),
             m_bluetooth(m_configuration),

             m_throttle(std::make_unique<Throttle>()),
             m_guardSwitch(std::make_unique<Switch>(m_configuration->getGuardSwitchPin())),
             m_breakSwitch(std::make_unique<Switch>(m_configuration->getBreakSwitchPin())),
             m_clutchSwitch(std::make_unique<Switch>(m_configuration->getClutchSwitchPin())),
             m_controller(std::make_unique<Controller>(m_configuration)),
             m_modeButton(std::make_unique<ModeButton>(m_configuration->getModeButtonPin())),
             m_twistPosition(std::make_unique<TwistPosition>()) {
}

void App::setup() {
  m_guardSwitch->registerSwitchCallback([this](bool const isEnabled) {
    m_controller->disable();
  });
  m_breakSwitch->registerSwitchCallback([this](bool const isEnabled) {
    if (isEnabled) {
      m_controller->modeDisable();
    }
  });
  m_clutchSwitch->registerSwitchCallback([this](bool const isEnabled) {
    if (isEnabled) {
      m_controller->modeDisable();
    }
  });
  m_controller->registerPositionUpdateCallback([this](Controller::Position const position) {
    m_throttle->setPosition(position);
    if (position > 20) {
      speed += 1;
    } else {
      speed -= 1;
    }
    m_controller->setSpeed(speed);
  });
  m_modeButton->registerHoldCallback([this] {
    m_controller->modeEnable();
  });
  m_modeButton->registerPressCallback([this] {
    m_controller->modeDisable();
  });
  m_twistPosition->registerPositionChangeCallback([this](TwistPosition::Position const position) {
    m_controller->setTwistPosition(position);
  });
}

void App::run() {
  m_bluetooth.advertise();

  m_executor.addNode(m_throttle, 1);
  m_executor.addNode(m_guardSwitch, 1);
  m_executor.addNode(m_breakSwitch ,1);
  m_executor.addNode(m_clutchSwitch ,1);
  m_executor.addNode(m_controller, 1);
  m_executor.addNode(m_modeButton);
  m_executor.addNode(m_twistPosition, 1);

  m_executor.spin();
}
