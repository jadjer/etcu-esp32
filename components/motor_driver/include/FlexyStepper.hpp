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

#include <cstdlib>
#include <climits>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

using PositionCallbackFunction = void (*)(int64_t);

/**
 * @class FlexyStepper
 */
class FlexyStepper {
public:
    /**
     * Connect the stepper object to the IO pins
     * @param stepsPerRevolution
     * @param directionPin IO pin number for the direction signal
     * @param stepPin IO pin number for the Step signal
     */
    FlexyStepper(uint32_t stepsPerRevolution, uint8_t directionPin, uint8_t stepPin);

    /**
     * Connect the stepper object to the IO pins
     * @param stepsPerRevolution
     * @param directionPin IO pin number for the direction signal
     * @param stepPin IO pin number for the Step signal
     * @param enablePin IO pin number for enable motor
     */
    FlexyStepper(uint32_t stepsPerRevolution, uint8_t directionPin, uint8_t stepPin, int8_t enablePin);

    /**
     * Destructor
     */
    ~FlexyStepper();

public:
    /**
     * register a callback function to be called whenever a target position has been reached
     * @param targetPositionReachedCallbackFunction
     */
    void registerTargetPositionReachedCallback(PositionCallbackFunction targetPositionReachedCallbackFunction);

public:
    /**
     *
     * @param ms1
     * @param ms2
     * @param ms3
     * @param useOpenDrain
     */
    void setMicroStepPins(uint64_t microSteps, uint8_t ms1, uint8_t ms2, uint8_t ms3);

    /**
     *
     * @param microStep
     */
    void setMicroSteps(uint64_t microSteps);

    /**
     * set the number of steps the motor has per revolution
     * @param motorStepPerRevolution
     */
    void setStepsPerRevolution(uint32_t motorStepPerRevolution);

    /**
     * set the maximum speed, units in steps/second, this is the maximum speed reached
     * while accelerating
     * Enter:  speedInStepsPerSecond = speed to accelerate up to, units in steps/second
     * @param speedInStepsPerSecond
     */
    void setSpeedInStepsPerSecond(uint32_t speedInStepsPerSecond);

    /**
     * set the rate of acceleration, units in steps/second/second
     * Enter:  accelerationInStepsPerSecond = rate of acceleration, units in
     * steps/second/second
     * @param accelerationInStepsPerSecond
     */
    void setAccelerationInStepsPerSecond(uint32_t accelerationInStepsPerSecond);

    /**
     * set the rate of deceleration, units in steps/second/second
     * Enter:  decelerationInStepsPerSecond = rate of deceleration, units in
     * steps/second/second
     * @param decelerationInStepsPerSecond
     */
    void setDecelerationInStepsPerSecond(uint32_t decelerationInStepsPerSecond);

    /**
     * set the current position of the motor in steps, this does not move the motor
     * currentPositionInSteps = the new position value of the motor in steps to be set internally for the current position
     * Do not confuse this function with setTargetPositionInMillimeters(), it does not directly cause a motor movement per se.
     * Notes:
     * This function should only be called when the motor is stopped
     * If you called one of the move functions before (and by that setting a target position internally) you might experience that the motor starts to move after calling setCurrentPositionInSteps() in the case that the value of currentPositionInSteps is different from the target position of the stepper.
     * If this is not intended, you should call setTargetPositionInSteps() with the same value as the setCurrentPositionInSteps() function directly before or after calling setCurrentPositionInSteps
     * @param currentPositionInSteps
     */
    void setCurrentPositionInSteps(int32_t currentPositionInSteps);

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
     * get the overall max stack size since task creation (since the call to startAsService() )
     * This function is used to determine if the stack size is large enough and has more of a debugging purpose.
     * @return the minimum amount of free bytes on the stack that has been measured so far.
     */
    uint32_t getTaskStackHighWaterMark();

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
    [[nodiscard]] double getCurrentVelocityInStepsPerSecond() const;

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
     * Check if running as service
     * @return
     */
    [[nodiscard]] bool isStartedAsService() const;

    /**
     * check if the motor has competed its move to the target position
     * Exit:  true returned if the stepper is at the target position
     * @return
     */
    [[nodiscard]] bool isMotionComplete() const;

public:
    /**
     * Start as service
     * @param coreNumber Number of cores
     * @return
     */
    bool startAsService(uint8_t coreNumber = 1);

    /**
     * Stop service
     */
    void stopService();

public:
    /**
     * if it is time, move one step
     * Exit:  true returned if movement complete, false returned not a final target position yet
     * @return
     */
    bool processMovement();

private:
    /**
     * determine the period for the next step, either speed up a little, slow down a
     * little or go the same speed
     */
    void determinePeriodOfNextStep();

private:
    [[noreturn]] static void taskRunner(void *parameter);

private:
    PositionCallbackFunction _targetPositionReachedCallback = nullptr;

private:
    bool _firstProcessingAfterTargetReached;

private:
    uint8_t _stepPin;
    uint8_t _directionPin;

private:
    int8_t _enablePin;
    int8_t _microStep1Pin;
    int8_t _microStep2Pin;
    int8_t _microStep3Pin;

private:
    int8_t _directionOfMotion;

private:
    int32_t _targetPosition_InSteps;
    int32_t _currentPosition_InSteps;

private:
    uint32_t _lastStepTime_InUS;
    uint32_t _nextStepPeriod_InUS;
    uint32_t _currentStepPeriod_InUS;
    uint32_t _periodOfSlowestStep_InUS;

private:
    uint32_t _microStepsPerStep;
    uint32_t _stepsPerRevolution;
    uint32_t _desiredSpeed_InStepsPerSecond;

private:
    uint32_t _acceleration_InStepsPerUS;
    uint32_t _deceleration_InStepsPerUS;
    uint32_t _desiredPeriod_InUSPerStep;
    uint32_t _minimumPeriodForAStoppedMotion;

private:
    TaskHandle_t _handle = nullptr;
};
