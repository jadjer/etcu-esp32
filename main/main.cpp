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

#include <cstdint>

//#include "executor/Executor.hpp"

#include "sdkconfig.h"

#include "NimBLEDevice.h"

//#include "Accelerator.hpp"
//#include "ModeButton.hpp"
//#include "SetupButton.hpp"
//#include "EtcController.hpp"
//#include "stepper/MotorController.hpp"

constexpr uint32_t const motorDefaultSpeed = 1500;
constexpr uint32_t const motorDefaultAcceleration = 15000;
constexpr uint32_t const motorDefaultDeceleration = 30000;

extern "C" void app_main(void) {
    NimBLEDevice::init("ETCU");
    NimBLEDevice::setMTU(517);
    NimBLEDevice::startAdvertising();

//  auto motorController = std::make_shared<MotorController>(500, motorDefaultSpeed, 500);
//  motorController->setSpeed(motorDefaultSpeed);
//  motorController->setAcceleration(motorDefaultAcceleration);
//  motorController->setDeceleration(motorDefaultDeceleration);
//  motorController->moveToHome();
//
//  auto etcController = std::make_shared<EtcController>();
//  etcController->registerChangeValueCallback(
//      [&](uint32_t const motorPosition) {
//        motorController->setPosition(static_cast<int32_t>(motorPosition));
//      });
//
//  auto accelerator = std::make_shared<Accelerator>();
//  accelerator->registerChangeAccelerateCallback(
//      [&](uint32_t const acceleratorValue_InPercentage) {
//        etcController->setAcceleratorValue(acceleratorValue_InPercentage);
//        throttlePositionCharacteristic->setValue(acceleratorValue_InPercentage);
//      });
//
//  auto setupButton = std::make_shared<SetupButton>();
//  setupButton->registerChangeValueCallback(
//      [&](SetupButtonState const setupButtonState) {
//        if (setupButtonState == SETUP_BUTTON_HELD) {
//          etcController->modeEnable();
//        }
//        if (setupButtonState == SETUP_BUTTON_PRESSED) {
//          etcController->modeDisable();
//        }
//      });
//
//  auto modeButton = std::make_shared<ModeButton>();
//  modeButton->registerChangeValueCallback(
//      [&](ModeButtonState const modeButtonState) {
//        float speedRate = 1.0;
//
//        if (modeButtonState == MODE_BUTTON_STATE_MODE_1) {
//          speedRate = 0.3;
//        }
//
//        if (modeButtonState == MODE_BUTTON_STATE_MODE_2) {
//          speedRate = 0.5;
//        }
//
//        if (modeButtonState == MODE_BUTTON_STATE_MODE_3) {
//          speedRate = 1.0;
//        }
//
//        auto const speed = motorDefaultSpeed * speedRate;
//
//        motorController->setSpeed(speed);
//      });

//  auto uart = std::make_unique<ECU::UartNetworkConnector>(3, 1, 2);
//  auto kLine = std::make_unique<ECU::KLineNetworkConnector>(1, std::move(uart));
//  auto ecu = std::make_shared<ECU::HondaECU>(std::move(kLine));

//  auto executor = std::make_unique<executor::Executor>();
//  executor->addNode(motorController, 300000);
//  executor->addNode(etcController, 1000);
//  executor->addNode(accelerator, 1000);
//  executor->addNode(setupButton, 1000);
//  executor->addNode(modeButton, 1000);
////  executor->addNode(ecu);
//  executor->spin();
}