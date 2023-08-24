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
#include <esp_log.h>

#include <executor/Executor.hpp>
#include <motor/MotorDriver.hpp>
#include <accelerator/Accelerator.hpp>
#include <indicator/BlinkIndicator.hpp>
#include <ecu/UartProtocol.hpp>

#include "Controller.hpp"
#include "SetupButton.hpp"
#include "ElectronicControlUnit.hpp"
#include "node/EcuNode.hpp"
#include "node/MotorNode.hpp"
#include "node/ControllerNode.hpp"
#include "node/AcceleratorNode.hpp"
#include "node/SetupButtonNode.hpp"

#include <vector>


int64_t averageFilter(int64_t const value, float const threshold) {
    static const size_t windowSize = 10;

    std::vector<int64_t> accelerationItems;
    accelerationItems.reserve(windowSize);

    accelerationItems[windowSize - 1] = value;

    for (size_t i = 0; i < windowSize - 1; i++) {
        accelerationItems[i] = accelerationItems[i + 1];
    }

    int64_t result = 0;

    for (auto const accelerationItem : accelerationItems) {
        result += accelerationItem;
    }

    result /= windowSize;

    if (std::fabs(value - result) > threshold)
    {
        return result;
    }

    return result;
}

int64_t expRunningAverageAdaptive(int64_t currentValue, float threshold) {
    static int64_t previousValue = 0;
    float k;

    auto diff = std::fabs(currentValue - previousValue);
    if (diff > threshold) {
        k = 0.9;
    } else {
        k = 0.001;
    }

    previousValue += static_cast<int64_t>(static_cast<float >(currentValue - previousValue) * k);
    return previousValue;
}

extern "C" void app_main(void) {
    auto indicatorPtr = std::make_shared<BlinkIndicator>(2);

    auto motorDriverPtr = std::make_shared<MotorDriver>(13, 12);
    motorDriverPtr->setAccelerationInStepsPerSecondPerSecond(200000);
    motorDriverPtr->setDecelerationInStepsPerSecondPerSecond(400000);
    motorDriverPtr->setSpeedInStepsPerSecond(100000);

    auto controllerPtr = std::make_shared<Controller>();
    controllerPtr->registerChangeValueCallback([&motorDriverPtr](int32_t controlValue) {
        auto diff_InSteps = std::fabs(motorDriverPtr->getTargetPositionInSteps() - controlValue);
        if (diff_InSteps < 16) {
            return;
        }

        motorDriverPtr->setTargetPositionInSteps(controlValue);
    });

    auto acceleratorPtr = std::make_shared<Accelerator>();
    acceleratorPtr->registerChangeAccelerateCallback([&controllerPtr](uint32_t accelerationValue) {
        accelerationValue = averageFilter(accelerationValue, 10);
        accelerationValue = expRunningAverageAdaptive(accelerationValue, 20);
        accelerationValue = accelerationValue * 4;

        controllerPtr->setAcceleration(accelerationValue);
    });

    auto setupButtonPtr = std::make_shared<SetupButton>(15, false);
    setupButtonPtr->registerChangeValueCallback([&indicatorPtr, &controllerPtr](SetupButtonState setupButtonState) {
        if (setupButtonState == SETUP_BUTTON_HELD) {
            controllerPtr->enable();
            indicatorPtr->enable();
        }
        if (setupButtonState == SETUP_BUTTON_PRESSED) {
            controllerPtr->disable();
            indicatorPtr->disable();
        }
    });

    auto ecuUartProtocol = std::make_unique<UartProtocol>(3, 1);
    ecuUartProtocol->connect();
    auto ecu = std::make_shared<ElectronicControlUnit>(std::move(ecuUartProtocol));

    auto controllerNodePtr = std::make_unique<ControllerNode>(controllerPtr);
    auto motorDriverNodePtr = std::make_unique<MotorNode>(motorDriverPtr);
    auto acceleratorNodePtr = std::make_unique<AcceleratorNode>(acceleratorPtr);
    auto setupButtonNodePtr = std::make_unique<SetupButtonNode>(setupButtonPtr);
    auto ecuNodePtr = std::make_unique<EcuNode>(ecu);

    auto executorPtr = std::make_unique<Executor>();
    executorPtr->addNode(std::move(controllerNodePtr));
    executorPtr->addNode(std::move(motorDriverNodePtr));
    executorPtr->addNode(std::move(acceleratorNodePtr));
    executorPtr->addNode(std::move(setupButtonNodePtr));
    executorPtr->addNode(std::move(ecuNodePtr));
    executorPtr->spin();
}
