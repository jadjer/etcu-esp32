// Copyright 2023 Pavel Suprunov
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

#include <cmath>
#include <vector>
#include <iostream>

#include "sdkconfig.h"

#include "executor/Executor.hpp"
#include "motor/MotorDriver.hpp"
#include "accelerator/Accelerator.hpp"
#include "accelerator/Scaler.hpp"
#include "accelerator/Formatter.hpp"
#include "indicator/BlinkIndicator.hpp"
#include "ecu/HondaECU.hpp"
#include "ecu/UartNetworkConnector.hpp"
#include "ecu/KLineNetworkConnector.hpp"

#include "Controller.hpp"
#include "ModeButton.hpp"
#include "SetupButton.hpp"

constexpr uint32_t const motorDefaultSpeed = (32 * 200 * 1000) / 60;
constexpr uint32_t const motorDefaultAcceleration = motorDefaultSpeed;
constexpr uint32_t const motorDefaultDeceleration = motorDefaultSpeed * 2;

constexpr uint32_t const acceleratorMinimalValue = 840;
constexpr uint32_t const acceleratorMaximalValue = 2570;
constexpr uint8_t const throttlePositionMinimalValue = 0;
constexpr uint8_t const throttlePositionMaximalValue = 100;

int64_t averageFilter(int64_t value, float threshold)
{
    static const size_t windowSize = 3;
    static int64_t data[windowSize];

    for (size_t i = 0; i < windowSize - 1; i++)
    {
        data[i] = data[i + 1];
    }
    data[windowSize - 1] = value;

    int64_t mean = 0;
    for (int64_t i : data)
    {
        mean += i;
    }
    mean /= windowSize;

    if (std::fabs(value - mean) > threshold)
    {
        return mean;
    }
    else
    {
        return value;
    }
}

extern "C" void app_main(void)
{
    auto motorDriverPtr = std::make_shared<motor::MotorDriver>(13, 11);
    motorDriverPtr->setFrequency(1000000);
    motorDriverPtr->setSpeedInStepsPerSecond(motorDefaultSpeed);
    motorDriverPtr->setAccelerationInStepsPerSecondPerSecond(motorDefaultAcceleration);
    motorDriverPtr->setDecelerationInStepsPerSecondPerSecond(motorDefaultDeceleration);

    auto controllerPtr = std::make_shared<Controller>();
    controllerPtr->registerChangeValueCallback(
        [&](uint32_t const acceleratorValue)
        {
            auto steps = static_cast<int32_t>(acceleratorValue * 115);
            std::cout << "| Motor: " << steps << " ";

            motorDriverPtr->setTargetPositionInSteps(steps);
        });

    auto scalerPtr = std::make_shared<accelerator::Scaler>(throttlePositionMinimalValue, throttlePositionMaximalValue);
    scalerPtr->registerChangeValueCallback(
        [&](uint32_t const scaledValue)
        {
            std::cout << "| Scaled value: " << scaledValue << " ";
            controllerPtr->setAcceleration(scaledValue);
        });

    auto formatterPtr = std::make_shared<accelerator::Formatter>(
        acceleratorMinimalValue, acceleratorMaximalValue, throttlePositionMinimalValue, throttlePositionMaximalValue);
    formatterPtr->registerChangeValueCallback(
        [&](uint32_t const formattedValue)
        {
            std::cout << "| Formatted value: " << formattedValue << " ";
            scalerPtr->setValue(formattedValue);
        });

    auto acceleratorPtr = std::make_shared<accelerator::Accelerator>();
    acceleratorPtr->setFrequency(8);
    acceleratorPtr->registerChangeAccelerateCallback(
        [&](uint32_t acceleratorValue_InPercentage)
        {
            std::cout << "Accelerator: " << acceleratorValue_InPercentage << "mV ";

            acceleratorValue_InPercentage = static_cast<uint32_t>(static_cast<float>(acceleratorValue_InPercentage) * 0.1) * 10;
            std::cout << "| Accelerator(filter1): " << acceleratorValue_InPercentage << " ";

            acceleratorValue_InPercentage = averageFilter(acceleratorValue_InPercentage, 20);
            std::cout << "| Accelerator(filter2): " << acceleratorValue_InPercentage << " ";

            formatterPtr->setValue(acceleratorValue_InPercentage);
            std::cout << std::endl;
        });

    auto setupButtonPtr = std::make_shared<SetupButton>(5, true);
    setupButtonPtr->setFrequency(100);
    setupButtonPtr->registerChangeValueCallback(
        [&](SetupButtonState const setupButtonState)
        {
            if (setupButtonState == SETUP_BUTTON_HELD)
            {
                scalerPtr->enable();
                controllerPtr->enable();
            }
            if (setupButtonState == SETUP_BUTTON_PRESSED)
            {
                scalerPtr->disable();
                controllerPtr->disable();
            }
        });

    auto modeButtonPtr = std::make_shared<ModeButton>(7, 6);
    modeButtonPtr->setFrequency(100);
    modeButtonPtr->registerChangeValueCallback(
        [&](ModeButtonState const modeButtonState)
        {
            float accelerationRate;

            switch (modeButtonState)
            {
            case MODE_BUTTON_STATE_MODE_1:
            {
                accelerationRate = 2.0;

                std::cout << std::endl << "Motor mode 1" << std::endl;

                break;
            }
            case MODE_BUTTON_STATE_MODE_2:
            {
                accelerationRate = 1.0;

                std::cout << std::endl << "Motor mode 2" << std::endl;

                break;
            }
            default:
            case MODE_BUTTON_STATE_MODE_3:
            {
                accelerationRate = 0.5;

                std::cout << std::endl << "Motor mode 3" << std::endl;

                break;
            }
            }

            auto acceleration = motorDefaultAcceleration * accelerationRate;

            motorDriverPtr->setAccelerationInStepsPerSecondPerSecond(acceleration);
            motorDriverPtr->setDecelerationInStepsPerSecondPerSecond(acceleration);
        });

    //    auto const numberOfUartPort = 2;
    //    auto const numberOfRxPin = 3;
    //    auto const numberOfTxPin = 1;
    //
    //    auto uartPtr = std::make_unique<ECU::UartNetworkConnector>(numberOfRxPin, numberOfTxPin, numberOfUartPort);
    //    auto kLinePtr = std::make_unique<ECU::KLineNetworkConnector>(numberOfTxPin, std::move(uartPtr));
    //    auto ecuPtr = std::make_shared<ECU::HondaECU>(std::move(kLinePtr));

    auto executorPtr = std::make_unique<executor::Executor>();
    executorPtr->addNode(motorDriverPtr);
    executorPtr->addNode(acceleratorPtr);
    executorPtr->addNode(setupButtonPtr);
    executorPtr->addNode(modeButtonPtr);
    //    executorPtr->addNode(ecuPtr);
    executorPtr->spin();
}
