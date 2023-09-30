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

#pragma once

#include "executor/Node.hpp"
#include "gpio/interface/IPin.hpp"
#include "motor/driver/interface/IDriver.hpp"

namespace motor
{

/**
 * @class MotorDriver
 */
class Motor : public executor::Node
{
public:
    /**
     * Connect the stepper object to the IO pins
     * @param driver Motor driver implementation
     */
    explicit Motor(IDriverPtr driver);

    /**
     * Destructor
     */
    ~Motor() override = default;

public:
    /**
     * set the maximum speed, units in steps/second, this is the maximum speed reached
     * while accelerating
     * Enter:  speedInStepsPerSecond = speed to accelerate up to, units in steps/second
     * @param speedInStepsPerSecond
     */
    void setSpeedInStepsPerSecond(float speedInStepsPerSecond);

    /**
     * set the rate of acceleration, units in steps/second/second
     * Enter:  accelerationInStepsPerSecond = rate of acceleration, units in
     * steps/second/second
     * @param accelerationInStepsPerSecond
     */
    void setAccelerationInStepsPerSecondPerSecond(float accelerationInStepsPerSecondPerSecond);

    /**
     * set the rate of deceleration, units in steps/second/second
     * Enter:  decelerationInStepsPerSecond = rate of deceleration, units in
     * steps/second/second
     * @param decelerationInStepsPerSecond
     */
    void setDecelerationInStepsPerSecondPerSecond(float decelerationInStepsPerSecondPerSecond);

    /**
     * setup a "Stop" to begin the process of decelerating from the current velocity
     * to zero, decelerating requires calls to processMove() until the move is complete
     * Note: This function can be used to stop a motion initiated in units of steps or revolutions
     */
    void setTargetPositionToStop();

    /**
     * setup a move, units are in steps, no motion occurs until processMove() is called
     * Enter:  absolutePositionToMoveToInSteps = signed absolute position to move to
     * in units of steps
     * @param absolutePositionToMoveToInSteps
     */
    void setTargetPositionInSteps(int32_t absolutePositionToMoveToInSteps);

    /**
     * setup a move relative to the current position, units are in steps, no motion
     * occurs until processMove() is called
     * Enter:  distanceToMoveInSteps = signed distance to move relative to the current
     * position in steps
     * @param distanceToMoveInSteps
     */
    void setTargetPositionRelativeInSteps(int32_t distanceToMoveInSteps);

public:
    /**
     * get the current direction of motion of the connected stepper motor
     * @return 1 for "forward" motion
     * @return -1 for "backward" motion
     * @return 0 if the stepper has reached its destination position and is not moving anymore
     */
    [[nodiscard]] int8_t getDirectionOfMotion() const;

    /**
     * Get the current velocity of the motor in steps/second.  This functions is
     * updated while it accelerates up and down in speed.  This is not the desired
     * speed, but the speed the motor should be moving at the time the function is
     * called.  This is a signed value and is negative when the motor is moving
     * backwards.  Note: This speed will be incorrect if the desired velocity is set
     * faster than this library can generate steps, or if the load on the motor is too
     * great for the amount of torque that it can generate.
     * Exit:  velocity speed in steps per second returned, signed
     * @return
     */
    [[nodiscard]] float getCurrentVelocityInStepsPerSecond() const;

    /**
     * get the current position of the motor in steps, this functions is updated
     * while the motor moves
     * Exit:  a signed motor position in steps returned
     * @return
     */
    [[nodiscard]] int32_t getCurrentPositionInSteps() const;

    /**
     * get the distance in steps to the currently set target position.
     * @return 0 if the stepper is already at the target position.
     * @return value is signed, depending on the direction to move to reach the target
     */
    [[nodiscard]] int64_t getDistanceToTargetSigned() const;

    /**
     *
     * @return
     */
    [[nodiscard]] int64_t getTargetPositionInSteps() const;

public:
    /**
     * check if the motor has competed its move to the target position
     * Exit:  true returned if the stepper is at the target position
     * @return
     */
    [[nodiscard]] bool isMotionComplete() const;

protected:
    void process() override;

private:
    /**
     * determine the period for the next step, either speed up a little, slow down a
     * little or go the same speed
     */
    uint32_t determinePeriodOfNextStep();

    /**
     *
     * @return
     */
    [[nodiscard]] uint32_t calcDecelerationDistanceInSteps() const;

private:
    int8_t _directionOfMotion;
    float _nextStepPeriod_InUS;
    uint64_t _lastStepTime_InUS;
    float _currentStepPeriod_InUS;
    float _periodOfSlowestStep_InUS;
    int32_t _targetPosition_InSteps;
    int32_t _currentPosition_InSteps;
    float _desiredPeriod_InUSPerStep;
    float _desiredSpeed_InStepsPerSecond;
    float _acceleration_InStepsPerUSPerUS;
    float _deceleration_InStepsPerUSPerUS;
    float _minimumPeriodForAStoppedMotion;

private:
    IDriverPtr m_driver;
};

} // namespace motor
