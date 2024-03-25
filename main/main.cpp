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

#include <iostream>
#include <cstdint>

#include "executor/Executor.hpp"

#include "MotorController.hpp"
#include "MotorDriver.hpp"
#include "Scaller.hpp"
#include "Formatter.hpp"
#include "Accelerator.hpp"
#include "Accelerator.hpp"
#include "ModeButton.hpp"
#include "SetupButton.hpp"
#include "Controller.hpp"

constexpr uint32_t const motorDefaultSpeed = 1000;
constexpr uint32_t const motorDefaultAcceleration = 10000;
constexpr uint32_t const motorDefaultDeceleration = 20000;

extern "C" void app_main(void) {
  auto motorController = std::make_shared<MotorController>();
  motorController->setFrequency(250000);
  motorController->setSpeed(motorDefaultSpeed);
  motorController->setAcceleration(motorDefaultAcceleration);
  motorController->setDeceleration(motorDefaultDeceleration);

//  auto etcController = std::make_shared<Controller>();
//  etcController->registerChangeValueCallback(
//      [&](uint32_t const acceleratorValue) {
//        auto steps = static_cast<int32_t>(acceleratorValue * 115);
//        std::cout << "| Motor: " << steps << " ";
//
//        motorController->setPosition(steps);
//      });

  auto accelerator = std::make_shared<Accelerator>();
  accelerator->setFrequency(100000);
  accelerator->registerChangeAccelerateCallback(
      [&](uint32_t acceleratorValue_InPercentage) {
        motorController->setPosition(acceleratorValue_InPercentage);
      });

//  auto setupButton = std::make_shared<SetupButton>();
//  setupButton->registerChangeValueCallback(
//      [&](SetupButtonState const setupButtonState) {
//        if (setupButtonState == SETUP_BUTTON_HELD) {
//          scaller->enable();
//          etcController->enable();
//        }
//        if (setupButtonState == SETUP_BUTTON_PRESSED) {
//          scaller->disable();
//          etcController->disable();
//        }
//      });

//  auto modeButtonPtr = std::make_shared<ModeButton>();
//  modeButtonPtr->registerChangeValueCallback(
//      [&](ModeButtonState const modeButtonState) {
//        float accelerationRate;
//
//        switch (modeButtonState) {
//        case MODE_BUTTON_STATE_MODE_1: {
//          accelerationRate = 2.0;
//          std::cout << std::endl << "Motor mode 1" << std::endl;
//
//          break;
//        }
//        case MODE_BUTTON_STATE_MODE_2: {
//          accelerationRate = 1.0;
//          std::cout << std::endl << "Motor mode 2" << std::endl;
//
//          break;
//        }
//        default:
//        case MODE_BUTTON_STATE_MODE_3: {
//          accelerationRate = 0.5;
//          std::cout << std::endl << "Motor mode 3" << std::endl;
//
//          break;
//        }
//        }
//
//        auto acceleration = motorDefaultAcceleration * accelerationRate;
//
//        motorController->setAcceleration(acceleration);
//        motorController->setDeceleration(acceleration);
//      });

//  auto uart = std::make_unique<ECU::UartNetworkConnector>(3, 1, 2);
//  auto kLine = std::make_unique<ECU::KLineNetworkConnector>(1, std::move(uart));
//  auto ecu = std::make_shared<ECU::HondaECU>(std::move(kLine));

  auto executor = std::make_unique<executor::Executor>();
  executor->addNode(motorController);
  executor->addNode(accelerator);
//  executor->addNode(setupButton);
//  executor->addNode(modeButtonPtr);
//  executor->addNode(ecu);
  executor->spin();
}