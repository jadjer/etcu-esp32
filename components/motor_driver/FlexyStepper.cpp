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

#include "FlexyStepper.hpp"

#include <cmath>

#include <esp_timer.h>
#include <driver/gpio.h>

auto pinConfiguration = [](uint8_t pinNumber) -> void {
    gpio_config_t stepPinConfig = {
            .pin_bit_mask = (1ull << pinNumber),
            .mode = GPIO_MODE_OUTPUT,
            .pull_up_en = GPIO_PULLUP_DISABLE,
            .pull_down_en = GPIO_PULLDOWN_ENABLE,
            .intr_type = GPIO_INTR_DISABLE,
    };

    gpio_config(&stepPinConfig);
};

auto micros = []() -> uint64_t {
    return esp_timer_get_time();
};

FlexyStepper::FlexyStepper(uint32_t stepsPerRevolution, uint8_t directionPin, uint8_t stepPin) :
        _firstProcessingAfterTargetReached(true),
        _stepPin(stepPin),
        _directionPin(directionPin),
        _enablePin(-1),
        _microStep1Pin(-1),
        _microStep2Pin(-1),
        _microStep3Pin(-1),
        _directionOfMotion(0),
        _targetPosition_InSteps(0),
        _currentPosition_InSteps(0),
        _lastStepTime_InUS(0),
        _nextStepPeriod_InUS(0),
        _currentStepPeriod_InUS(0),
        _periodOfSlowestStep_InUS(0),
        _microStepsPerStep(1),
        _stepsPerRevolution(stepsPerRevolution),
        _desiredSpeed_InStepsPerSecond(0),
        _acceleration_InStepsPerUS(0),
        _deceleration_InStepsPerUS(0),
        _desiredPeriod_InUSPerStep(0),
        _minimumPeriodForAStoppedMotion(0) {

    setSpeedInStepsPerSecond(200);
    setAccelerationInStepsPerSecond(200);
    setDecelerationInStepsPerSecond(200);

    pinConfiguration(_directionPin);
    pinConfiguration(_stepPin);
}

FlexyStepper::FlexyStepper(uint32_t stepsPerRevolution, uint8_t directionPin, uint8_t stepPin, int8_t enablePin)
        : FlexyStepper(stepsPerRevolution, directionPin, stepPin) {
    assert(enablePin > 0);

    _enablePin = enablePin;

    pinConfiguration(_enablePin);
}

FlexyStepper::~FlexyStepper() {
    if (_handle != nullptr) {
        stopService();
    }
}

void FlexyStepper::registerTargetPositionReachedCallback(PositionCallbackFunction targetPositionReachedCallbackFunction) {
    _targetPositionReachedCallback = targetPositionReachedCallbackFunction;
}

void FlexyStepper::setMicroStepPins(uint64_t microStepsPerStep, uint8_t ms1, uint8_t ms2, uint8_t ms3) {
    _microStep1Pin = static_cast<int8_t>(ms1);
    _microStep2Pin = static_cast<int8_t>(ms2);
    _microStep3Pin = static_cast<int8_t>(ms3);

    pinConfiguration(_microStep1Pin);
    pinConfiguration(_microStep2Pin);
    pinConfiguration(_microStep3Pin);

    setMicroStepsPerStep(microStepsPerStep);
}

void FlexyStepper::setMicroStepsPerStep(uint64_t microStepsPerStep) {
    _microStepsPerStep = microStepsPerStep;
}

void FlexyStepper::setStepsPerRevolution(uint32_t stepsPerRevolution) {
    _stepsPerRevolution = stepsPerRevolution;
}

void FlexyStepper::setSpeedInStepsPerSecond(uint32_t speedInStepsPerSecond) {
    _desiredSpeed_InStepsPerSecond = speedInStepsPerSecond;
    _desiredPeriod_InUSPerStep = 1e6 / _desiredSpeed_InStepsPerSecond;
}

void FlexyStepper::setAccelerationInStepsPerSecond(uint32_t accelerationInStepsPerSecond) {
    _acceleration_InStepsPerUS = static_cast<double>(accelerationInStepsPerSecond) / 1e12;

    _periodOfSlowestStep_InUS = static_cast<uint32_t>(std::round(1.0e6 / sqrt(2.0 * accelerationInStepsPerSecond)));
    _minimumPeriodForAStoppedMotion = static_cast<double>(_periodOfSlowestStep_InUS) / 2.8;
}

void FlexyStepper::setDecelerationInStepsPerSecond(uint32_t decelerationInStepsPerSecond) {
    _deceleration_InStepsPerUS = static_cast<double>(decelerationInStepsPerSecond) / 1e12;
}

void FlexyStepper::setCurrentPositionInSteps(int32_t currentPositionInSteps) {
    _currentPosition_InSteps = currentPositionInSteps;
}

void FlexyStepper::setTargetPositionToStop() {
    if (_directionOfMotion == 0) {
        return;
    }

    int32_t decelerationDistance_InSteps;

    //
    // move the target position so that the motor will begin deceleration now
    //
    decelerationDistance_InSteps = (int32_t) std::round(5E11 / (_deceleration_InStepsPerUS * _currentStepPeriod_InUS * _currentStepPeriod_InUS));

    if (_directionOfMotion > 0) {
        setTargetPositionInSteps(_currentPosition_InSteps + decelerationDistance_InSteps);
    } else {
        setTargetPositionInSteps(_currentPosition_InSteps - decelerationDistance_InSteps);
    }
}

void FlexyStepper::setTargetPositionInSteps(int32_t absolutePositionToMoveToInSteps) {
    _targetPosition_InSteps = absolutePositionToMoveToInSteps;
    _firstProcessingAfterTargetReached = true;
}

void FlexyStepper::setTargetPositionRelativeInSteps(int32_t distanceToMoveInSteps) {
    setTargetPositionInSteps(_currentPosition_InSteps + distanceToMoveInSteps);
}

uint32_t FlexyStepper::getTaskStackHighWaterMark() {
    if (isStartedAsService()) {
        return uxTaskGetStackHighWaterMark(_handle);
    }

    return 0;
}

int8_t FlexyStepper::getDirectionOfMotion() const {
    return _directionOfMotion;
}

double FlexyStepper::getCurrentVelocityInStepsPerSecond() const {
    if (_currentStepPeriod_InUS == 0) {
        return 0;
    }

    if (_directionOfMotion > 0) {
        return 1e6 / _currentStepPeriod_InUS;
    }

    return -1e6 / _currentStepPeriod_InUS;
}

int32_t FlexyStepper::getCurrentPositionInSteps() const {
    return _currentPosition_InSteps;
}

int64_t FlexyStepper::getDistanceToTargetSigned() const {
    return _targetPosition_InSteps - _currentPosition_InSteps;
}

int64_t FlexyStepper::getTargetPositionInSteps() const {
    return _targetPosition_InSteps;
}

bool FlexyStepper::isStartedAsService() const {
    return _handle != nullptr;
}

bool FlexyStepper::isMotionComplete() const {
    if (_directionOfMotion == 0 and _currentPosition_InSteps == _targetPosition_InSteps) {
        return true;
    } else {
        return false;
    }
}

bool FlexyStepper::startAsService(uint8_t coreNumber) {
    xTaskCreatePinnedToCore(FlexyStepper::taskRunner, "FlexyStepper", 2000, this, 1, &_handle, coreNumber);

    configASSERT(_handle);
    return true;
}

void FlexyStepper::stopService() {
    vTaskDelete(_handle);

    _handle = nullptr;
}

bool FlexyStepper::processMovement() {
    int64_t distanceToTarget_Signed;

    if (_directionOfMotion == 0) { // check if currently stopped
        distanceToTarget_Signed = _targetPosition_InSteps - _currentPosition_InSteps;
        if (distanceToTarget_Signed > 0) { // check if target position in a positive direction
            _directionOfMotion = 1;

            gpio_set_level(static_cast<gpio_num_t>(_directionPin), 0);

            _nextStepPeriod_InUS = _periodOfSlowestStep_InUS;
            _lastStepTime_InUS = micros();

            return false;

        } else if (distanceToTarget_Signed < 0) { // check if target position in a negative direction
            _directionOfMotion = -1;

            gpio_set_level(static_cast<gpio_num_t>(_directionPin), 1);

            _nextStepPeriod_InUS = _periodOfSlowestStep_InUS;
            _lastStepTime_InUS = micros();

            return false;

        } else {
            return true;
        }
    }

    // determine how much time has elapsed since the last step (Note 1: this method
    // works even if the time has wrapped. Note 2: all variables must be unsigned)
    uint64_t currentTime_InUS = micros();
    uint64_t periodSinceLastStep_InUS = currentTime_InUS - _lastStepTime_InUS;
    // if it is not time for the next step, return
    if (periodSinceLastStep_InUS < _nextStepPeriod_InUS) {
        return false;
    }

    // execute the step on the rising edge
    gpio_set_level(static_cast<gpio_num_t>(_stepPin), 1);

    // update the current position and speed
    _currentPosition_InSteps += _directionOfMotion;
    _currentStepPeriod_InUS = _nextStepPeriod_InUS;

    // remember the time that this step occurred
    _lastStepTime_InUS = currentTime_InUS;

    // figure out how long before the next step
    determinePeriodOfNextStep();

    // return the step line low
    gpio_set_level(static_cast<gpio_num_t>(_stepPin), 0);

    if (_currentPosition_InSteps != _targetPosition_InSteps) { // check if the move has reached its final target position, return true if all done
        return false;
    }

    if (_nextStepPeriod_InUS < _minimumPeriodForAStoppedMotion) { // at final position, make sure the motor is not going too fast
        return false;
    }

    _currentStepPeriod_InUS = 0.0;
    _nextStepPeriod_InUS = 0.0;
    _directionOfMotion = 0;

    if (_firstProcessingAfterTargetReached) {
        _firstProcessingAfterTargetReached = false;

        if (_targetPositionReachedCallback) {
            _targetPositionReachedCallback(_currentPosition_InSteps);
        }
    }

    return true;
}

uint32_t FlexyStepper::determinePeriodOfNextStep() {
    uint32_t distanceToTarget_Unsigned;
    bool speedUpFlag = false;
    bool slowDownFlag = false;
    bool targetInPositiveDirectionFlag = false;
    bool targetInNegativeDirectionFlag = false;

    auto distanceToTarget_Signed = _targetPosition_InSteps - _currentPosition_InSteps;
    if (distanceToTarget_Signed >= 0) {
        distanceToTarget_Unsigned = distanceToTarget_Signed;
        targetInPositiveDirectionFlag = true;
    } else {
        distanceToTarget_Unsigned = -distanceToTarget_Signed;
        targetInNegativeDirectionFlag = true;
    }

    //
    // determine the number of steps needed to go from the current speed down to a
    // velocity of 0, Steps = Velocity^2 / (2 * Deceleration)
    //
    auto currentStepPeriodSquared = _currentStepPeriod_InUS * _currentStepPeriod_InUS;

    auto decelerationDistance_InSteps = std::round(5E11 / (_deceleration_InStepsPerUS * currentStepPeriodSquared));

    if (_directionOfMotion == 1 and targetInPositiveDirectionFlag) {
        if (distanceToTarget_Unsigned < decelerationDistance_InSteps or _nextStepPeriod_InUS < _desiredPeriod_InUSPerStep) {
            slowDownFlag = true;
        } else {
            speedUpFlag = true;
        }

    } else if (_directionOfMotion == 1 and targetInNegativeDirectionFlag) {
        //
        // need to slow down, then reverse direction
        //
        if (_currentStepPeriod_InUS < _periodOfSlowestStep_InUS) {
            slowDownFlag = true;
        } else {
            _directionOfMotion = -1;

            gpio_set_level(static_cast<gpio_num_t>(_directionPin), 1);
        }

    } else if (_directionOfMotion == -1 and targetInNegativeDirectionFlag) {
        //
        // check if need to start slowing down as we reach the target, or if we
        // need to slow down because we are going too fast
        //
        if (distanceToTarget_Unsigned < decelerationDistance_InSteps or _nextStepPeriod_InUS < _desiredPeriod_InUSPerStep) {
            slowDownFlag = true;
        } else {
            speedUpFlag = true;
        }

    } else if (_directionOfMotion == -1 and targetInPositiveDirectionFlag) {
        //
        // need to slow down, then reverse direction
        //
        if (_currentStepPeriod_InUS < _periodOfSlowestStep_InUS) {
            slowDownFlag = true;
        } else {
            _directionOfMotion = 1;

            gpio_set_level(static_cast<gpio_num_t>(_directionPin), 0);
        }
    }

    if (speedUpFlag) {
        // StepPeriod = StepPeriod(1 - a * StepPeriod^2)
        _nextStepPeriod_InUS = _currentStepPeriod_InUS - _currentStepPeriod_InUS * _acceleration_InStepsPerUS * currentStepPeriodSquared;

        if (_nextStepPeriod_InUS < _desiredPeriod_InUSPerStep) {
            _nextStepPeriod_InUS = _desiredPeriod_InUSPerStep;
        }
    }

    if (slowDownFlag) {
        // StepPeriod = StepPeriod(1 + a * StepPeriod^2)
        _nextStepPeriod_InUS = _currentStepPeriod_InUS + _currentStepPeriod_InUS * _deceleration_InStepsPerUS * currentStepPeriodSquared;

        if (_nextStepPeriod_InUS > _periodOfSlowestStep_InUS) {
            _nextStepPeriod_InUS = _periodOfSlowestStep_InUS;
        }
    }

    return _nextStepPeriod_InUS;
}

[[noreturn]] void FlexyStepper::taskRunner(void *parameter) {
    auto stepperRef = static_cast<FlexyStepper *>(parameter);
    for (;;) {
        stepperRef->processMovement();
        // vTaskDelay(1); // This would be a working solution to prevent the WDT to fire (if not disabled, yet it will cause noticeably less smooth stepper movements / lower frequencies)
    }
}
