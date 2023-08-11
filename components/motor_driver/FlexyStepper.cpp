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

#include <driver/gpio.h>
#include <cmath>

//
// direction signal level for "step and direction"
//
#define POSITIVE_DIRECTION LOW
#define NEGATIVE_DIRECTION HIGH

auto pinConfiguration = [](uint8_t pinNumber) {
    gpio_config_t stepPinConfig = {
            .pin_bit_mask = (1ULL << pinNumber),
            .mode = GPIO_MODE_OUTPUT,
            .pull_up_en = GPIO_PULLUP_DISABLE,
            .pull_down_en = GPIO_PULLDOWN_ENABLE,
            .intr_type = GPIO_INTR_DISABLE,
    };

    gpio_config(&stepPinConfig);
};

FlexyStepper::FlexyStepper(uint8_t stepsPerRevolution, uint8_t directionPin, uint8_t stepPin) {
    _stepsPerRevolution = stepsPerRevolution;
    _directionPin = directionPin;
    _stepPin = stepPin;

    _lastStepTime_InUS = 0L;
    _directionOfMotion = 0;
    _currentPosition_InSteps = 0L;
    _targetPosition_InSteps = 0L;
    _currentStepPeriod_InUS = 0.0;
    _nextStepPeriod_InUS = 0.0;
    _emergencyStopActive = false;
    _isCurrentlyHomed = false;
    _directionTowardsHome = -1;
    _disallowedDirection = 0;
    _activeLimitSwitch = 0; // see LIMIT_SWITCH_BEGIN and LIMIT_SWITCH_END
    _lastStepDirectionBeforeLimitSwitchTrigger = 0;
    _limitSwitchCheckPerformed = false;
    _holdEmergencyStopUntilExplicitRelease = false;

    setSpeedInStepsPerSecond(200);
    setAccelerationInStepsPerSecond(200.0);
    setDecelerationInStepsPerSecond(200.0);

    pinConfiguration(_directionPin);
    pinConfiguration(_stepPin);
}

FlexyStepper::FlexyStepper(uint8_t stepsPerRevolution, uint8_t directionPin, uint8_t stepPin, uint8_t enablePin)
        : FlexyStepper(stepsPerRevolution, directionPin, stepPin) {
    _enablePin = enablePin;

    pinConfiguration(_enablePin);
}

FlexyStepper::~FlexyStepper() {
    if (_handle != nullptr) {
        stopService();
    }
}

void FlexyStepper::setMicroStepPins(uint8_t ms1, uint8_t ms2, uint8_t ms3) {
    _microStep1Pin = ms1;
    _microStep2Pin = ms2;
    _microStep3Pin = ms3;
}

bool FlexyStepper::startAsService(uint8_t coreNumber) {
    if (coreNumber == 1) {
//        disableCore1WDT(); // we have to disable the Watchdog timer to prevent it from rebooting the ESP all the time another option would be to add a vTaskDelay but it would slow down the stepper
    } else if (coreNumber == 0) {
//        disableCore0WDT(); // we have to disable the Watchdog timer to prevent it from rebooting the ESP all the time another option would be to add a vTaskDelay but it would slow down the stepper
    } else {
        // invalid core number given
        return false;
    }

    xTaskCreatePinnedToCore(
            FlexyStepper::taskRunner, /* Task function. */
            "FlexyStepper",               /* String with name of task (by default max 16 characters long) */
            2000,                         /* Stack size in bytes. */
            this,                         /* Parameter passed as input of the task */
            1,                            /* Priority of the task, 1 seems to work just fine for us */
            &_handle,               /* Task handle. */
            coreNumber                    /* the cpu core to use, 1 is where usually the Arduino Framework code (setup and loop function) are running, core 0 by default runs the Wifi Stack */
    );

    configASSERT(_handle);
    return true;
}

void FlexyStepper::stopService() {
    vTaskDelete(_handle);
    _handle = nullptr;
}

bool FlexyStepper::isStartedAsService() {
    return (_handle != nullptr);
}

uint32_t FlexyStepper::getTaskStackHighWaterMark() {
    if (isStartedAsService()) {
        return uxTaskGetStackHighWaterMark(_handle);
    }

    return 0;
}

bool FlexyStepper::processMovement() {
    if (_emergencyStopActive) {
        // abort potentially running homing movement
        _isOnWayToHome = false;
        _isOnWayToLimit = false;

        _currentStepPeriod_InUS = 0.0;
        _nextStepPeriod_InUS = 0.0;
        _directionOfMotion = 0;
        _targetPosition_InSteps = _currentPosition_InSteps;

        if (not _holdEmergencyStopUntilExplicitRelease) {
            _emergencyStopActive = false;
        }

        return true;
    }

    long distanceToTarget_Signed;

    if (_activeLimitSwitch != 0) { // check if limit switch flag is active
        distanceToTarget_Signed = _targetPosition_InSteps - _currentPosition_InSteps;

        if (not _limitSwitchCheckPerformed) {
            _limitSwitchCheckPerformed = true;

            // a limit switch is active, so movement is only allowed in one direction (away from the switch)
            if (_activeLimitSwitch == LIMIT_SWITCH_BEGIN) {
                _disallowedDirection = _directionTowardsHome;
            } else if (_activeLimitSwitch == LIMIT_SWITCH_END) {
                _disallowedDirection = _directionTowardsHome * -1;
            } else if (_activeLimitSwitch == LIMIT_SWITCH_COMBINED_BEGIN_AND_END) {
                // limit switches are paired together, so we need to try to figure out by checking which one it is, by using the last used step direction
                if (distanceToTarget_Signed > 0) {
                    _lastStepDirectionBeforeLimitSwitchTrigger = 1;
                    _disallowedDirection = 1;
                } else if (distanceToTarget_Signed < 0) {
                    _lastStepDirectionBeforeLimitSwitchTrigger = -1;
                    _disallowedDirection = -1;
                }
            }

            // movement has been triggered by goToLimitAndSetAsHome() function. so once the limit switch has been triggered we have reached the limit and need to set it as home
            if (_isOnWayToHome) {
                setCurrentPositionAsHomeAndStop(); // clear isOnWayToHome flag and stop motion

                if (_homeReachedCallback != nullptr) {
                    _homeReachedCallback();
                }

                return true;
            }
        }

        // check if further movement is allowed
        if ((_disallowedDirection == 1 and distanceToTarget_Signed > 0) or (_disallowedDirection == -1 and distanceToTarget_Signed < 0)) {
            // limit switch is active and movement in request direction is not allowed
            _currentStepPeriod_InUS = 0.0;
            _nextStepPeriod_InUS = 0.0;
            _directionOfMotion = 0;
            _targetPosition_InSteps = _currentPosition_InSteps;

            return true;
        }
    }

    uint32_t currentTime_InUS;
    uint32_t periodSinceLastStep_InUS;

    if (_directionOfMotion == 0) { // check if currently stopped
        distanceToTarget_Signed = _targetPosition_InSteps - _currentPosition_InSteps;
        if (distanceToTarget_Signed > 0) { // check if target position in a positive direction
            _directionOfMotion = 1;
            digitalWrite(_directionPin, POSITIVE_DIRECTION);
            _nextStepPeriod_InUS = _periodOfSlowestStep_InUS;
            _lastStepTime_InUS = micros();
            _lastStepDirectionBeforeLimitSwitchTrigger = _directionOfMotion;

            return false;

        } else if (distanceToTarget_Signed < 0) { // check if target position in a negative direction
            _directionOfMotion = -1;
            digitalWrite(_directionPin, NEGATIVE_DIRECTION);
            _nextStepPeriod_InUS = _periodOfSlowestStep_InUS;
            _lastStepTime_InUS = micros();
            _lastStepDirectionBeforeLimitSwitchTrigger = _directionOfMotion;

            return false;

        } else {
            _lastStepDirectionBeforeLimitSwitchTrigger = 0;

            return true;
        }
    }

    // determine how much time has elapsed since the last step (Note 1: this method
    // works even if the time has wrapped. Note 2: all variables must be unsigned)
    currentTime_InUS = micros();
    periodSinceLastStep_InUS = currentTime_InUS - _lastStepTime_InUS;
    // if it is not time for the next step, return
    if (periodSinceLastStep_InUS < (uint32_t) _nextStepPeriod_InUS) {
        return false;
    }

    // execute the step on the rising edge
    digitalWrite(_stepPin, HIGH);

    // update the current position and speed
    _currentPosition_InSteps += _directionOfMotion;
    _currentStepPeriod_InUS = _nextStepPeriod_InUS;

    // remember the time that this step occured
    _lastStepTime_InUS = currentTime_InUS;

    // figure out how long before the next step
    determinePeriodOfNextStep();

    // return the step line low
    digitalWrite(_stepPin, LOW);

    if (_currentPosition_InSteps != _targetPosition_InSteps) { // check if the move has reached its final target position, return true if all done
        return false;
    }

    if (_nextStepPeriod_InUS < _minimumPeriodForAStoppedMotion) { // at final position, make sure the motor is not going too fast
        return false;
    }

    _currentStepPeriod_InUS = 0.0;
    _nextStepPeriod_InUS = 0.0;
    _directionOfMotion = 0;
    _lastStepDirectionBeforeLimitSwitchTrigger = 0;

    if (_firstProcessingAfterTargetReached) {
        _firstProcessingAfterTargetReached = false;

        if (_targetPositionReachedCallback) {
            _targetPositionReachedCallback(_currentPosition_InSteps);
        }
    }

    return true;
}

void FlexyStepper::clearLimitSwitchActive() {
    _activeLimitSwitch = 0;
}

void FlexyStepper::emergencyStop(bool holdUntilReleased) {
    _holdEmergencyStopUntilExplicitRelease = holdUntilReleased;
    _emergencyStopActive = (not isMotionComplete() or _holdEmergencyStopUntilExplicitRelease);

    if (_emergencyStopTriggeredCallback) {
        _emergencyStopTriggeredCallback();
    }
}

void FlexyStepper::releaseEmergencyStop() {
    _emergencyStopActive = false;

    if (_emergencyStopReleasedCallback) {
        _emergencyStopReleasedCallback();
    }
}

bool FlexyStepper::isMotionComplete() const {
    if (_directionOfMotion == 0 and _currentPosition_InSteps == _targetPosition_InSteps) {
        return true;
    } else {
        return false;
    }
}

int8_t FlexyStepper::getDirectionOfMotion() const {
    return _directionOfMotion;
}

bool FlexyStepper::isMovingTowardsHome() const {
    return _directionOfMotion == _directionTowardsHome;
}

void FlexyStepper::registerHomeReachedCallback(CallbackFunction newFunction) {
    _homeReachedCallback = newFunction;
}

void FlexyStepper::registerLimitReachedCallback(CallbackFunction limitSwitchTriggeredCallbackFunction) {
    _limitTriggeredCallback = limitSwitchTriggeredCallbackFunction;
}

void FlexyStepper::registerEmergencyStopTriggeredCallback(CallbackFunction emergencyStopTriggerdCallbackFunction) {
    _emergencyStopTriggeredCallback = emergencyStopTriggerdCallbackFunction;
}

void FlexyStepper::registerEmergencyStopReleasedCallback(CallbackFunction emergencyStopReleasedCallbackFunction) {
    _emergencyStopReleasedCallback = emergencyStopReleasedCallbackFunction;
}

void FlexyStepper::registerTargetPositionReachedCallback(PositionCallbackFunction targetPositionReachedCallbackFunction) {
    _targetPositionReachedCallback = targetPositionReachedCallbackFunction;
}

void FlexyStepper::setStepsPerRevolution(float motorStepPerRevolution) {
    _stepsPerRevolution = motorStepPerRevolution;
}

void FlexyStepper::setSpeedInStepsPerSecond(float speedInStepsPerSecond) {
    _desiredSpeed_InStepsPerSecond = speedInStepsPerSecond;
    _desiredPeriod_InUSPerStep = 1000000 / _desiredSpeed_InStepsPerSecond;
}

void FlexyStepper::setAccelerationInStepsPerSecond(float accelerationInStepsPerSecond) {
    _acceleration_InStepsPerSecond = accelerationInStepsPerSecond;
    _acceleration_InStepsPerUSPerUS = _acceleration_InStepsPerSecond / 1E12;

    _periodOfSlowestStep_InUS = 1000000 / sqrt(2.0 * _acceleration_InStepsPerSecond);
    _minimumPeriodForAStoppedMotion = _periodOfSlowestStep_InUS / 2.8;
}

void FlexyStepper::setDecelerationInStepsPerSecond(float decelerationInStepsPerSecond) {
    _deceleration_InStepsPerSecond = decelerationInStepsPerSecond;
    _deceleration_InStepsPerUSPerUS = _deceleration_InStepsPerSecond / 1E12;
}

void FlexyStepper::setDirectionToHome(int8_t directionTowardHome) {
    if (directionTowardHome == -1 or directionTowardHome == 1) {
        _directionTowardsHome = directionTowardHome;
    }
}

void FlexyStepper::setLimitSwitchActive(int8_t limitSwitchType) {
    if (limitSwitchType == LIMIT_SWITCH_BEGIN or limitSwitchType == LIMIT_SWITCH_END or limitSwitchType == LIMIT_SWITCH_COMBINED_BEGIN_AND_END) {
        _activeLimitSwitch = limitSwitchType;
        _limitSwitchCheckPerformed = false; // set flag for newly set limit switch trigger

        if (_limitTriggeredCallback) {
            _limitTriggeredCallback();
        }
    }
}

float FlexyStepper::getCurrentVelocityInStepsPerSecond() const {
    if (_currentStepPeriod_InUS == 0.0) {
        return 0;
    }

    if (_directionOfMotion > 0) {
        return 1000000 / _currentStepPeriod_InUS;
    }

    return -1000000 / _currentStepPeriod_InUS;
}

void FlexyStepper::setCurrentPositionInSteps(int32_t currentPositionInSteps) {
    _currentPosition_InSteps = currentPositionInSteps;
}

int32_t FlexyStepper::getCurrentPositionInSteps() const {
    return _currentPosition_InSteps;
}

void FlexyStepper::startJogging(int8_t direction) {
    setTargetPositionInSteps(direction * 2000000000);
}

void FlexyStepper::stopJogging() {
    setTargetPositionToStop();
}

void FlexyStepper::setCurrentPositionAsHomeAndStop() {
    _isOnWayToHome = false;
    _currentStepPeriod_InUS = 0.0;
    _nextStepPeriod_InUS = 0.0;
    _directionOfMotion = 0;
    _currentPosition_InSteps = 0;
    _targetPosition_InSteps = 0;
    _isCurrentlyHomed = true;
}

void FlexyStepper::setTargetPositionToStop() {
    // abort potentially running homing movement
    _isOnWayToHome = false;
    _isOnWayToLimit = false;

    if (_directionOfMotion == 0) {
        return;
    }

    int32_t decelerationDistance_InSteps;

    //
    // move the target position so that the motor will begin deceleration now
    //
    decelerationDistance_InSteps = (int32_t) std::round(5E11 / (_deceleration_InStepsPerSecond * _currentStepPeriod_InUS * _currentStepPeriod_InUS));

    if (_directionOfMotion > 0) {
        setTargetPositionInSteps(_currentPosition_InSteps + decelerationDistance_InSteps);
    } else {
        setTargetPositionInSteps(_currentPosition_InSteps - decelerationDistance_InSteps);
    }
}

void FlexyStepper::setTargetPositionInSteps(int32_t absolutePositionToMoveToInSteps) {
    // abort potentially running homing movement
    _isOnWayToHome = false;
    _isOnWayToLimit = false;
    _targetPosition_InSteps = absolutePositionToMoveToInSteps;
    _firstProcessingAfterTargetReached = true;
}

void FlexyStepper::setTargetPositionRelativeInSteps(int32_t distanceToMoveInSteps) {
    setTargetPositionInSteps(_currentPosition_InSteps + distanceToMoveInSteps);
}

int32_t FlexyStepper::getDistanceToTargetSigned() const {
    return _targetPosition_InSteps - _currentPosition_InSteps;
}

int32_t FlexyStepper::getTargetPositionInSteps() const {
    return _targetPosition_InSteps;
}

void FlexyStepper::moveToPositionInSteps(int32_t absolutePositionToMoveToInSteps) {
    setTargetPositionInSteps(absolutePositionToMoveToInSteps);

    while (not processMovement());
}

void FlexyStepper::moveRelativeInSteps(int32_t distanceToMoveInSteps) {
    setTargetPositionRelativeInSteps(distanceToMoveInSteps);

    while (not processMovement());
}

void FlexyStepper::determinePeriodOfNextStep() {
    int32_t distanceToTarget_Signed;
    uint32_t distanceToTarget_Unsigned;
    int32_t decelerationDistance_InSteps;
    float currentStepPeriodSquared;
    bool speedUpFlag = false;
    bool slowDownFlag = false;
    bool targetInPositiveDirectionFlag = false;
    bool targetInNegativeDirectionFlag = false;

    //
    // determine the distance to the target position
    //
    distanceToTarget_Signed = _targetPosition_InSteps - _currentPosition_InSteps;
    if (distanceToTarget_Signed >= 0L) {
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
    currentStepPeriodSquared = _currentStepPeriod_InUS * _currentStepPeriod_InUS;
    decelerationDistance_InSteps = (int32_t) std::round(5E11 / (_deceleration_InStepsPerSecond * currentStepPeriodSquared));

    //
    // check if: Moving in a positive direction & Moving toward the target
    //    (directionOfMotion == 1) && (distanceToTarget_Signed > 0)
    //
    if (_directionOfMotion == 1 and targetInPositiveDirectionFlag) {
        //
        // check if need to start slowing down as we reach the target, or if we
        // need to slow down because we are going too fast
        //
        if (distanceToTarget_Unsigned < decelerationDistance_InSteps or _nextStepPeriod_InUS < _desiredPeriod_InUSPerStep) {
            slowDownFlag = true;
        } else {
            speedUpFlag = true;
        }

    } else if (_directionOfMotion == 1 and targetInNegativeDirectionFlag) {
        //
        // check if: Moving in a positive direction & Moving away from the target
        //    (directionOfMotion == 1) && (distanceToTarget_Signed < 0)
        //

        //
        // need to slow down, then reverse direction
        //
        if (_currentStepPeriod_InUS < _periodOfSlowestStep_InUS) {
            slowDownFlag = true;
        } else {
            _directionOfMotion = -1;
            digitalWrite(_directionPin, NEGATIVE_DIRECTION);
        }

    } else if (_directionOfMotion == -1 and targetInNegativeDirectionFlag) {
        //
        // check if: Moving in a negative direction & Moving toward the target
        //    (directionOfMotion == -1) && (distanceToTarget_Signed < 0)
        //

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
        // check if: Moving in a negative direction & Moving away from the target
        //    (directionOfMotion == -1) && (distanceToTarget_Signed > 0)
        //

        //
        // need to slow down, then reverse direction
        //
        if (_currentStepPeriod_InUS < _periodOfSlowestStep_InUS) {
            slowDownFlag = true;
        } else {
            _directionOfMotion = 1;
            digitalWrite(_directionPin, POSITIVE_DIRECTION);
        }
    }

    //
    // check if accelerating
    //
    if (speedUpFlag) {
        //
        // StepPeriod = StepPeriod(1 - a * StepPeriod^2)
        //
        _nextStepPeriod_InUS = _currentStepPeriod_InUS - _acceleration_InStepsPerUSPerUS * currentStepPeriodSquared * _currentStepPeriod_InUS;

        if (_nextStepPeriod_InUS < _desiredPeriod_InUSPerStep) {
            _nextStepPeriod_InUS = _desiredPeriod_InUSPerStep;
        }
    }

    //
    // check if decelerating
    //
    if (slowDownFlag) {
        //
        // StepPeriod = StepPeriod(1 + a * StepPeriod^2)
        //
        _nextStepPeriod_InUS = _currentStepPeriod_InUS + _deceleration_InStepsPerUSPerUS * currentStepPeriodSquared * _currentStepPeriod_InUS;

        if (_nextStepPeriod_InUS > _periodOfSlowestStep_InUS) {
            _nextStepPeriod_InUS = _periodOfSlowestStep_InUS;
        }
    }
}

[[noreturn]] void FlexyStepper::taskRunner(void *parameter) {
    auto stepperRef = static_cast<FlexyStepper *>(parameter);
    for (;;) {
        stepperRef->processMovement();
        // vTaskDelay(1); // This would be a working solution to prevent the WDT to fire (if not disabled, yet it will cause noticeably less smooth stepper movements / lower frequencies)
    }
}
