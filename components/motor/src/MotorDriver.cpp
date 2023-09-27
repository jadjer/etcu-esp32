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

#include <motor/MotorDriver.hpp>

#include <cmath>

#include <esp_timer.h>
#include <driver/gpio.h>

constexpr uint8_t lowLevel = 0;
constexpr uint8_t highLevel = 1;

constexpr uint8_t motorMode0 = 10;
constexpr uint8_t motorMode1 = 9;
constexpr uint8_t motorMode2 = 20;
constexpr uint8_t motorEnable = 12;
constexpr uint8_t motorDecay = 14;
constexpr uint8_t motorSleep = 47;
constexpr uint8_t motorReset = 48;
constexpr uint8_t motorFault = 21;
constexpr uint8_t motorHome = 19;

void pinOutputConfiguration(uint8_t numberOfPin, uint8_t defaultLevel = lowLevel)
{
    gpio_config_t pinConfig = {
        .pin_bit_mask = (1ull << numberOfPin),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_ENABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };

    if (defaultLevel == highLevel) {
        pinConfig.pull_up_en = GPIO_PULLUP_ENABLE;
        pinConfig.pull_down_en = GPIO_PULLDOWN_DISABLE;
    }

    gpio_config(&pinConfig);
    gpio_set_level(static_cast<gpio_num_t>(numberOfPin), defaultLevel);
}

MotorDriver::MotorDriver(uint8_t directionPin, uint8_t stepPin) :
    _stepPin(stepPin), _directionPin(directionPin), _directionOfMotion(0), _nextStepPeriod_InUS(0.0), _lastStepTime_InUS(0),
    _currentStepPeriod_InUS(0.0), _periodOfSlowestStep_InUS(0.0), _targetPosition_InSteps(0), _currentPosition_InSteps(0),
    _desiredPeriod_InUSPerStep(0.0), _desiredSpeed_InStepsPerSecond(0.0), _acceleration_InStepsPerUSPerUS(0.0),
    _deceleration_InStepsPerUSPerUS(0.0), _minimumPeriodForAStoppedMotion(0.0)
{

    setSpeedInStepsPerSecond(200);
    setAccelerationInStepsPerSecondPerSecond(200);
    setDecelerationInStepsPerSecondPerSecond(200);

    pinOutputConfiguration(motorMode0, highLevel);
    pinOutputConfiguration(motorMode1, highLevel);
    pinOutputConfiguration(motorMode2, highLevel);
    pinOutputConfiguration(motorEnable, lowLevel);
    pinOutputConfiguration(motorReset, highLevel);
    pinOutputConfiguration(motorSleep, highLevel);
    pinOutputConfiguration(motorDecay, highLevel);

    pinOutputConfiguration(_directionPin, lowLevel);
    pinOutputConfiguration(_stepPin, lowLevel);
}

void MotorDriver::setSpeedInStepsPerSecond(float speedInStepsPerSecond)
{
    _desiredSpeed_InStepsPerSecond = speedInStepsPerSecond;
    _desiredPeriod_InUSPerStep = 1e6 / _desiredSpeed_InStepsPerSecond;
}

void MotorDriver::setAccelerationInStepsPerSecondPerSecond(float accelerationInStepsPerSecondPerSecond)
{
    _acceleration_InStepsPerUSPerUS = accelerationInStepsPerSecondPerSecond / 1e12;

    _periodOfSlowestStep_InUS = 1e6 / sqrt(2.0 * accelerationInStepsPerSecondPerSecond);
    _minimumPeriodForAStoppedMotion = _periodOfSlowestStep_InUS / 2.8;
}

void MotorDriver::setDecelerationInStepsPerSecondPerSecond(float decelerationInStepsPerSecondPerSecond)
{
    _deceleration_InStepsPerUSPerUS = decelerationInStepsPerSecondPerSecond / 1e12;
}

void MotorDriver::setTargetPositionToStop()
{
    if (_directionOfMotion == 0)
    {
        return;
    }

    auto decelerationDistance_InSteps = calcDecelerationDistanceInSteps();

    if (_directionOfMotion > 0)
    {
        setTargetPositionInSteps(_currentPosition_InSteps + decelerationDistance_InSteps);
    }
    else
    {
        setTargetPositionInSteps(_currentPosition_InSteps - decelerationDistance_InSteps);
    }
}

void MotorDriver::setTargetPositionInSteps(int32_t absolutePositionToMoveToInSteps)
{
    _targetPosition_InSteps = absolutePositionToMoveToInSteps;
}

void MotorDriver::setTargetPositionRelativeInSteps(int32_t distanceToMoveInSteps)
{
    setTargetPositionInSteps(_currentPosition_InSteps + distanceToMoveInSteps);
}

int8_t MotorDriver::getDirectionOfMotion() const
{
    return _directionOfMotion;
}

double MotorDriver::getCurrentVelocityInStepsPerSecond() const
{
    if (_currentStepPeriod_InUS == 0)
    {
        return 0;
    }

    if (_directionOfMotion > 0)
    {
        return 1e6 / _currentStepPeriod_InUS;
    }

    return -1e6 / _currentStepPeriod_InUS;
}

int32_t MotorDriver::getCurrentPositionInSteps() const
{
    return _currentPosition_InSteps;
}

int64_t MotorDriver::getDistanceToTargetSigned() const
{
    return _targetPosition_InSteps - _currentPosition_InSteps;
}

int64_t MotorDriver::getTargetPositionInSteps() const
{
    return _targetPosition_InSteps;
}

bool MotorDriver::isMotionComplete() const
{
    if (_directionOfMotion == 0 and _currentPosition_InSteps == _targetPosition_InSteps)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void MotorDriver::process()
{
    int64_t distanceToTarget_Signed;

    if (_directionOfMotion == 0)
    { // check if currently stopped
        distanceToTarget_Signed = _targetPosition_InSteps - _currentPosition_InSteps;
        if (distanceToTarget_Signed > 0)
        { // check if target position in a positive direction
            _directionOfMotion = 1;

            gpio_set_level(static_cast<gpio_num_t>(_directionPin), lowLevel);

            _nextStepPeriod_InUS = _periodOfSlowestStep_InUS;
            _lastStepTime_InUS = esp_timer_get_time();

            return;
        }
        else if (distanceToTarget_Signed < 0)
        { // check if target position in a negative direction
            _directionOfMotion = -1;

            gpio_set_level(static_cast<gpio_num_t>(_directionPin), highLevel);

            _nextStepPeriod_InUS = _periodOfSlowestStep_InUS;
            _lastStepTime_InUS = esp_timer_get_time();

            return;
        }

        return;
    }

    // determine how much time has elapsed since the last step (Note 1: this method
    // works even if the time has wrapped. Note 2: all variables must be unsigned)
    uint64_t currentTime_InUS = esp_timer_get_time();
    uint64_t periodSinceLastStep_InUS = currentTime_InUS - _lastStepTime_InUS;
    // if it is not time for the next step, return
    if (periodSinceLastStep_InUS < _nextStepPeriod_InUS)
    {
        return;
    }

    // execute the step on the rising edge
    gpio_set_level(static_cast<gpio_num_t>(_stepPin), highLevel);

    // update the current position and speed
    _currentPosition_InSteps += _directionOfMotion;
    _currentStepPeriod_InUS = _nextStepPeriod_InUS;

    // remember the time that this step occurred
    _lastStepTime_InUS = currentTime_InUS;

    // figure out how long before the next step
    determinePeriodOfNextStep();

    // return the step line low
    gpio_set_level(static_cast<gpio_num_t>(_stepPin), lowLevel);

    if (_currentPosition_InSteps != _targetPosition_InSteps)
    { // check if the move has reached its final target position, return true if all done
        return;
    }

    if (_nextStepPeriod_InUS < _minimumPeriodForAStoppedMotion)
    { // at final position, make sure the motor is not going too fast
        return;
    }

    _currentStepPeriod_InUS = 0.0;
    _nextStepPeriod_InUS = 0.0;
    _directionOfMotion = 0;
}

uint32_t MotorDriver::determinePeriodOfNextStep()
{
    uint32_t distanceToTarget_Unsigned;
    bool speedUpFlag = false;
    bool slowDownFlag = false;
    bool targetInPositiveDirectionFlag = false;
    bool targetInNegativeDirectionFlag = false;

    auto distanceToTarget_Signed = _targetPosition_InSteps - _currentPosition_InSteps;
    if (distanceToTarget_Signed >= 0)
    {
        distanceToTarget_Unsigned = distanceToTarget_Signed;
        targetInPositiveDirectionFlag = true;
    }
    else
    {
        distanceToTarget_Unsigned = -distanceToTarget_Signed;
        targetInNegativeDirectionFlag = true;
    }

    auto currentStepPeriodSquared = _currentStepPeriod_InUS * _currentStepPeriod_InUS;
    auto decelerationDistance_InSteps = calcDecelerationDistanceInSteps();

    if (_directionOfMotion == 1 and targetInPositiveDirectionFlag)
    {
        if (distanceToTarget_Unsigned < decelerationDistance_InSteps or _nextStepPeriod_InUS < _desiredPeriod_InUSPerStep)
        {
            slowDownFlag = true;
        }
        else
        {
            speedUpFlag = true;
        }
    }
    else if (_directionOfMotion == 1 and targetInNegativeDirectionFlag)
    {
        //
        // need to slow down, then reverse direction
        //
        if (_currentStepPeriod_InUS < _periodOfSlowestStep_InUS)
        {
            slowDownFlag = true;
        }
        else
        {
            _directionOfMotion = -1;

            gpio_set_level(static_cast<gpio_num_t>(_directionPin), highLevel);
        }
    }
    else if (_directionOfMotion == -1 and targetInNegativeDirectionFlag)
    {
        //
        // check if need to start slowing down as we reach the target, or if we
        // need to slow down because we are going too fast
        //
        if (distanceToTarget_Unsigned < decelerationDistance_InSteps or _nextStepPeriod_InUS < _desiredPeriod_InUSPerStep)
        {
            slowDownFlag = true;
        }
        else
        {
            speedUpFlag = true;
        }
    }
    else if (_directionOfMotion == -1 and targetInPositiveDirectionFlag)
    {
        //
        // need to slow down, then reverse direction
        //
        if (_currentStepPeriod_InUS < _periodOfSlowestStep_InUS)
        {
            slowDownFlag = true;
        }
        else
        {
            _directionOfMotion = 1;

            gpio_set_level(static_cast<gpio_num_t>(_directionPin), lowLevel);
        }
    }

    if (speedUpFlag)
    {
        // StepPeriod = StepPeriod(1 - a * StepPeriod^2)
        _nextStepPeriod_InUS = _currentStepPeriod_InUS -
            _currentStepPeriod_InUS * _acceleration_InStepsPerUSPerUS * currentStepPeriodSquared;

        if (_nextStepPeriod_InUS < _desiredPeriod_InUSPerStep)
        {
            _nextStepPeriod_InUS = _desiredPeriod_InUSPerStep;
        }
    }

    if (slowDownFlag)
    {
        // StepPeriod = StepPeriod(1 + a * StepPeriod^2)
        _nextStepPeriod_InUS = _currentStepPeriod_InUS +
            _currentStepPeriod_InUS * _deceleration_InStepsPerUSPerUS * currentStepPeriodSquared;

        if (_nextStepPeriod_InUS > _periodOfSlowestStep_InUS)
        {
            _nextStepPeriod_InUS = _periodOfSlowestStep_InUS;
        }
    }

    return _nextStepPeriod_InUS;
}

#include <esp_log.h>

uint32_t MotorDriver::calcDecelerationDistanceInSteps() const
{
    //
    // determine the number of steps needed to go from the current speed down to a
    // velocity of 0, Steps = Velocity^2 / (2 * Deceleration)
    //
    auto currentStepPeriodSquared = _currentStepPeriod_InUS * _currentStepPeriod_InUS;
    auto decelerationDistance = std::round(1 / (_deceleration_InStepsPerUSPerUS * currentStepPeriodSquared));

    auto decelerationDistance_InSteps = static_cast<uint32_t>(decelerationDistance);
    return decelerationDistance_InSteps;
}
