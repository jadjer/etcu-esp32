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

App::App() : m_configuration(std::make_shared<Configuration>()),
             m_bluetooth(m_configuration),
             m_controller(m_configuration),

             m_throttle(std::make_unique<Throttle>()),
             m_modeButton(std::make_unique<ModeButton>(m_configuration->getModeButtonPin())),
             m_twistPosition(std::make_unique<TwistPosition>()) {
}

void App::setup() {
  m_controller.registerChangeThrottlePositionCallback([this](std::uint8_t const position) { m_throttle->setPosition(position); });
  m_twistPosition->registerPositionChangedCallback([this](std::uint8_t const position) { m_controller.setTwistPosition(position); });
  m_modeButton->registerLongPressedCallback([this]() { m_controller.modeButtonLongPressed(); });
  m_modeButton->registerShortPressedCallback([this]() { m_controller.modeButtonShortPressed(); });
}

void App::run() {
  m_bluetooth.advertise();

  m_executor.addNode(m_throttle, 1);
  m_executor.addNode(m_modeButton, 1);
  m_executor.addNode(m_twistPosition, 1);

  m_executor.spin();
}
