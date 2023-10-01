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

#include "motor/Motor.hpp"

#include <cmath>

#include "esp_timer.h"

#include "gpio/Pin.hpp"

namespace motor
{

Motor::Motor(IDriverPtr driver) :
    _directionOfMotion(0), _nextStepPeriod_InUS(0.0), _lastStepTime_InUS(0), _currentStepPeriod_InUS(0.0),
    _periodOfSlowestStep_InUS(0.0), _targetPosition_InSteps(0), _currentPosition_InSteps(0), _desiredPeriod_InUSPerStep(0.0),
    _desiredSpeed_InStepsPerSecond(0.0), _acceleration_InStepsPerUSPerUS(0.0), _deceleration_InStepsPerUSPerUS(0.0),
    _minimumPeriodForAStoppedMotion(0.0), m_driver(std::move(driver))
{
    setSpeedInStepsPerSecond(200);
    setAccelerationInStepsPerSecondPerSecond(200);
    setDecelerationInStepsPerSecondPerSecond(200);
}

void Motor::setSpeedInStepsPerSecond(float speedInStepsPerSecond)
{
    _desiredSpeed_InStepsPerSecond = speedInStepsPerSecond;
    _desiredPeriod_InUSPerStep = 1000000 / _desiredSpeed_InStepsPerSecond;
}

void Motor::setAccelerationInStepsPerSecondPerSecond(float accelerationInStepsPerSecondPerSecond)
{
    _acceleration_InStepsPerUSPerUS = static_cast<float>(accelerationInStepsPerSecondPerSecond / 1000000000000.0);

    _periodOfSlowestStep_InUS = static_cast<float>(1000000 / sqrt(2.0 * accelerationInStepsPerSecondPerSecond));
    _minimumPeriodForAStoppedMotion = static_cast<float>(_periodOfSlowestStep_InUS / 2.8);
}

void Motor::setDecelerationInStepsPerSecondPerSecond(float decelerationInStepsPerSecondPerSecond)
{
    _deceleration_InStepsPerUSPerUS = static_cast<float>(decelerationInStepsPerSecondPerSecond / 1000000000000.0);
}

void Motor::setTargetPositionToStop()
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

void Motor::setTargetPositionInSteps(int32_t absolutePositionToMoveToInSteps)
{
    _targetPosition_InSteps = absolutePositionToMoveToInSteps;
}

void Motor::setTargetPositionRelativeInSteps(int32_t distanceToMoveInSteps)
{
    setTargetPositionInSteps(_currentPosition_InSteps + distanceToMoveInSteps);
}

int8_t Motor::getDirectionOfMotion() const
{
    return _directionOfMotion;
}

float Motor::getCurrentVelocityInStepsPerSecond() const
{
    if (_currentStepPeriod_InUS == 0)
    {
        return 0;
    }

    if (_directionOfMotion > 0)
    {
        return 1000000 / _currentStepPeriod_InUS;
    }

    return -1000000 / _currentStepPeriod_InUS;
}

int32_t Motor::getCurrentPositionInSteps() const
{
    return _currentPosition_InSteps;
}

int64_t Motor::getDistanceToTargetSigned() const
{
    return _targetPosition_InSteps - _currentPosition_InSteps;
}

int64_t Motor::getTargetPositionInSteps() const
{
    return _targetPosition_InSteps;
}

bool Motor::isMotionComplete() const
{
    if (_directionOfMotion != 0) {
        return false;
    }

    if (_currentPosition_InSteps ==! _targetPosition_InSteps) {
        return false;
    }

    return true;
}

void Motor::process()
{
    int64_t distanceToTarget_Signed;

    if (_directionOfMotion == 0)
    { // check if currently stopped
        distanceToTarget_Signed = _targetPosition_InSteps - _currentPosition_InSteps;
        if (distanceToTarget_Signed > 0)
        { // check if target position in a positive direction
            _directionOfMotion = 1;

            m_driver->setDirection(motor::driver::MOTOR_ROTATE_CLOCKWISE);

            _nextStepPeriod_InUS = _periodOfSlowestStep_InUS;
            _lastStepTime_InUS = esp_timer_get_time();

            return;
        }
        else if (distanceToTarget_Signed < 0)
        { // check if target position in a negative direction
            _directionOfMotion = -1;

            m_driver->setDirection(motor::driver::MOTOR_ROTATE_OTHERWISE);

            _nextStepPeriod_InUS = _periodOfSlowestStep_InUS;
            _lastStepTime_InUS = esp_timer_get_time();

            return;
        }

        return;
    }

    // determine how much time has elapsed since the last step (Note 1: this method
    // works even if the time has wrapped. Note 2: all variables must be unsigned)
    uint64_t const currentTime_InUS = esp_timer_get_time();
    uint64_t const periodSinceLastStep_InUS = currentTime_InUS - _lastStepTime_InUS;
    // if it is not time for the next step, return
    if (periodSinceLastStep_InUS < _nextStepPeriod_InUS)
    {
        return;
    }

    // execute the step on the rising edge
    m_driver->setStepToHigh();

    // update the current position and speed
    _currentPosition_InSteps += _directionOfMotion;
    _currentStepPeriod_InUS = _nextStepPeriod_InUS;

    // remember the time that this step occurred
    _lastStepTime_InUS = currentTime_InUS;

    // figure out how long before the next step
    determinePeriodOfNextStep();

    // return the step line low
    m_driver->setStepToLow();

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

uint32_t Motor::determinePeriodOfNextStep()
{
    uint32_t distanceToTarget_Unsigned;
    bool speedUpFlag = false;
    bool slowDownFlag = false;
    bool targetInPositiveDirectionFlag = false;
    bool targetInNegativeDirectionFlag = false;

    auto const distanceToTarget_Signed = _targetPosition_InSteps - _currentPosition_InSteps;
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

    auto const currentStepPeriodSquared = _currentStepPeriod_InUS * _currentStepPeriod_InUS;
    auto const decelerationDistance_InSteps = calcDecelerationDistanceInSteps();

    if (_directionOfMotion == 1) {
        if (targetInPositiveDirectionFlag) {
            if (distanceToTarget_Unsigned < decelerationDistance_InSteps or _nextStepPeriod_InUS < _desiredPeriod_InUSPerStep)
            {
                slowDownFlag = true;
            }
            else
            {
                speedUpFlag = true;
            }
        }

        if (targetInNegativeDirectionFlag) {
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

                m_driver->setDirection(motor::driver::MOTOR_ROTATE_OTHERWISE);
            }
        }
    }

    if (_directionOfMotion == -1) {
        if (targetInPositiveDirectionFlag) {
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

                m_driver->setDirection(motor::driver::MOTOR_ROTATE_CLOCKWISE);
            }
        }

        if (targetInNegativeDirectionFlag) {
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

uint32_t Motor::calcDecelerationDistanceInSteps() const
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

} // namespace motor
