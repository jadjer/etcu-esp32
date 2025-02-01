//
// Created by jadjer on 01.02.25.
//

#pragma once

#include "Controller.hpp"
#include "ModeButton.hpp"
#include "configuration/interface/Configuration.hpp"
#include "sensor/TwistPosition.hpp"
#include <executor/Executor.hpp>

using ModeButtonPtr = std::shared_ptr<ModeButton>;
using TwistPositionPtr = std::shared_ptr<TwistPosition>;

class App {
public:
  App();

public:
  void setup();
  void run();

private:
  ConfigurationPtr const m_configuration;

  private:
  Controller m_controller;
  executor::Executor m_executor;

private:
  ModeButtonPtr m_modeButton = nullptr;
  TwistPositionPtr m_twistPosition = nullptr;
};
