//
// Created by jadjer on 01.02.25.
//

#include "App.hpp"

#include "configuration/Configuration.hpp"

App::App() : m_configuration(std::make_shared<Configuration>()),
             m_controller(m_configuration),
             m_modeButton(std::make_unique<ModeButton>(m_configuration->getModeButtonPin())),
             m_twistPosition(std::make_unique<TwistPosition>()) {
}

void App::setup() {
  m_modeButton->registerLongPressedCallback([this]() { m_controller.modeButtonLongPressed(); });
  m_modeButton->registerShortPressedCallback([this]() { m_controller.modeButtonShortPressed(); });
  m_twistPosition->registerPositionChangedCallback([this](std::uint8_t const position) { m_controller.setTwistPosition(position); });
}

void App::run() {
  m_executor.addNode(m_modeButton);
  m_executor.addNode(m_twistPosition, 1);
  m_executor.spin();
}
