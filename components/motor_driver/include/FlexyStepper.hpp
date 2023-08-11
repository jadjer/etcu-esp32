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

using CallbackFunction = void (*)();
using PositionCallbackFunction = void (*)(int32_t);

/**
 * @class FlexyStepper
 */
class FlexyStepper {
public:
    static const int8_t LIMIT_SWITCH_BEGIN = -1;
    static const int8_t LIMIT_SWITCH_END = 1;
    static const int8_t LIMIT_SWITCH_COMBINED_BEGIN_AND_END = 2;
    static const uint8_t ACTIVE_HIGH = 1;
    static const uint8_t ACTIVE_LOW = 2;

public:
    /**
     * Connect the stepper object to the IO pins
     * @param stepsPerRevolution
     * @param directionPin IO pin number for the direction signal
     * @param stepPin IO pin number for the Step signal
     */
    FlexyStepper(uint8_t stepsPerRevolution, uint8_t directionPin, uint8_t stepPin);

    /**
     * Connect the stepper object to the IO pins
     * @param stepsPerRevolution
     * @param directionPin IO pin number for the direction signal
     * @param stepPin IO pin number for the Step signal
     * @param enablePin IO pin number for enable motor
     */
    FlexyStepper(uint8_t stepsPerRevolution, uint8_t directionPin, uint8_t stepPin, uint8_t enablePin);

    /**
     * Destructor
     */
    ~FlexyStepper();

public:
    /**
     *
     * @param ms1
     * @param ms2
     * @param ms3
     * @param useOpenDrain
     */
    void setMicroStepPins(uint8_t ms1, uint8_t ms2, uint8_t ms3);

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

    /**
     * Check if running as service
     * @return
     */
    bool isStartedAsService();

    /**
     * get the overall max stack size since task creation (since the call to startAsService() )
     * This function is used to determine if the stack size is large enough and has more of a debugging purpose.
     * @return the minimum amount of free bytes on the stack that has been measured so far.
     */
    uint32_t getTaskStackHighWaterMark();

public:
    /**
     * if it is time, move one step
     * Exit:  true returned if movement complete, false returned not a final target position yet
     * @return
     */
    bool processMovement();

    /**
     * clear the limit switch flag to allow movement in both directions again
     */
    void clearLimitSwitchActive();

    /**
     * perform an emergency stop, causing all movements to be canceled instantly
     * the optional parameter 'holdUntilReleased' allows to define if the emergency stop shall only affect the current motion (if any)
     * or if it should hold the emergency stop status (kind of a latching functionality) until the releaseEmergencyStop() function is called explicitly.
     * Default for holdUntilReleased is false (if parameter is omitted)
     * @param holdUntilReleased
     */
    void emergencyStop(bool holdUntilReleased = false);

    /**
     * releases an emergency stop that has previously been engaged using a call to emergencyStop(true)
     */
    void releaseEmergencyStop();

public:
    /**
     * check if the motor has competed its move to the target position
     * Exit:  true returned if the stepper is at the target position
     * @return
     */
    [[nodiscard]] bool isMotionComplete() const;

    /**
     * get the current direction of motion of the connected stepper motor
     * @return 1 for "forward" motion
     * @return -1 for "backward" motion
     * @return 0 if the stepper has reached its destination position and is not moving anymore
     */
    [[nodiscard]] int8_t getDirectionOfMotion() const;

    /**
     * returns true if the stepper is currently in motion and moving in the direction of the home position.
     * Depends on the settings of setDirectionToHome() which defines where "home" is...a rather philosophical question :-)
     * @return
     */
    [[nodiscard]] bool isMovingTowardsHome() const;

public:
    /**
     * register a callback function to be called whenever a movement to home has been completed (does not trigger when movement passes by the home position)
     * @param homeReachedCallbackFunction
     */
    void registerHomeReachedCallback(CallbackFunction homeReachedCallbackFunction);

    /**
     * register a callback function to be called whenever a
     * @param limitSwitchTriggeredCallbackFunction
     */
    void registerLimitReachedCallback(CallbackFunction limitSwitchTriggeredCallbackFunction);

    /**
     * register a callback function to be called whenever a emergency stop is triggered
     * @param emergencyStopTriggeredCallbackFunction
     */
    void registerEmergencyStopTriggeredCallback(CallbackFunction emergencyStopTriggeredCallbackFunction);

    /**
     * register a callback function to be called whenever the emergency stop switch is released
     * @param emergencyStopReleasedCallbackFunction
     */
    void registerEmergencyStopReleasedCallback(CallbackFunction emergencyStopReleasedCallbackFunction);

    /**
     * register a callback function to be called whenever a target position has been reached
     * @param targetPositionReachedCallbackFunction
     */
    void registerTargetPositionReachedCallback(PositionCallbackFunction targetPositionReachedCallbackFunction);

public:
    /**
     * set the number of steps the motor has per revolution
     * @param motorStepPerRevolution
     */
    void setStepsPerRevolution(float motorStepPerRevolution);

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
    void setAccelerationInStepsPerSecond(float accelerationInStepsPerSecond);

    /**
     * set the rate of deceleration, units in steps/second/second
     * Enter:  decelerationInStepsPerSecond = rate of deceleration, units in
     * steps/second/second
     * @param decelerationInStepsPerSecond
     */
    void setDecelerationInStepsPerSecond(float decelerationInStepsPerSecond);

    /**
     * configure the direction in which to move to reach the home position
     * Accepts 1 or -1 as allowed values. Other values will be ignored
     * @param directionTowardHome
     */
    void setDirectionToHome(int8_t directionTowardHome);

    /**
     * Notification of an externally detected limit switch activation
     * Accepts LIMIT_SWITCH_BEGIN (-1) or LIMIT_SWITCH_END (1) as parameter values to indicate
     * whether the limit switch near the begin (direction of home position) or at the end of the movement has ben triggered.
     * It is strongly recommended to perform debouncing before calling this function to prevent issues when button is
     * released and re-triggering the limit switch function
     * @param limitSwitchType
     */
    void setLimitSwitchActive(int8_t limitSwitchType);

public:
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

public:
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
     * get the current position of the motor in steps, this functions is updated
     * while the motor moves
     * Exit:  a signed motor position in steps returned
     * @return
     */
    [[nodiscard]] int32_t getCurrentPositionInSteps() const;

public:
    /**
     * start jogging (continuous movement without a fixed target position)
     * uses the currently set speed and acceleration settings
     * to stop the motion call the stopJogging function.
     * Will also stop when the external limit switch has been triggered using setLimitSwitchActive() or when the emergencyStop function is triggered
     * Warning: This function requires either a limit switch to be configured or manual trigger of the stopJogging/setTargetPositionToStop
     * or emergencyStop function, otherwise the motor will never stop jogging (which could of course also be an intended behavior)
     * @param direction
     */
    void startJogging(int8_t direction);

    /**
     * Stop jogging, basically an alias function for setTargetPositionToStop()
     */
    void stopJogging();

public:
    /**
     * set the current position as the home position (Step count = 0)
     */
    void setCurrentPositionAsHomeAndStop();

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
     * get the distance in steps to the currently set target position.
     * @return 0 if the stepper is already at the target position.
     * @return value is signed, depending on the direction to move to reach the target
     */
    [[nodiscard]] int32_t getDistanceToTargetSigned() const;

    /**
     *
     * @return
     */
    [[nodiscard]] int32_t getTargetPositionInSteps() const;

public:
    /**
     * move to the given absolute position, units are in steps, this function does not
     * return until the move is complete
     * Enter:  absolutePositionToMoveToInSteps = signed absolute position to move to
     * in units of steps
     * @param absolutePositionToMoveToInSteps
     */
    void moveToPositionInSteps(int32_t absolutePositionToMoveToInSteps);

    /**
     * move relative to the current position, units are in steps, this function does
     * not return until the move is complete
     * Enter:  distanceToMoveInSteps = signed distance to move relative to the current
     * position in steps
     * @param distanceToMoveInSteps
     */
    void moveRelativeInSteps(int32_t distanceToMoveInSteps);

private:
    /**
     * determine the period for the next step, either speed up a little, slow down a
     * little or go the same speed
     */
    void determinePeriodOfNextStep();

private:
    [[noreturn]] static void taskRunner(void *parameter);

private:
    CallbackFunction _homeReachedCallback = nullptr;
    CallbackFunction _limitTriggeredCallback = nullptr;
    CallbackFunction _callbackFunctionForGoToLimit = nullptr;
    CallbackFunction _emergencyStopReleasedCallback = nullptr;
    CallbackFunction _emergencyStopTriggeredCallback = nullptr;
    PositionCallbackFunction _targetPositionReachedCallback = nullptr;

private:
    bool _isOnWayToHome;
    bool _isOnWayToLimit;
    bool _isCurrentlyHomed;
    bool _emergencyStopActive;
    bool _limitSwitchCheckPerformed;
    bool _firstProcessingAfterTargetReached;
    bool _holdEmergencyStopUntilExplicitRelease;

private:
    uint8_t _stepPin;
    uint8_t _resetPin;
    uint8_t _enablePin;
    uint8_t _directionPin;
    uint8_t _microStep1Pin;
    uint8_t _microStep2Pin;
    uint8_t _microStep3Pin;

private:
    int8_t _activeLimitSwitch;
    int8_t _directionOfMotion;
    int8_t _disallowedDirection;
    int8_t _directionTowardsHome;
    int8_t _lastStepDirectionBeforeLimitSwitchTrigger;

private:
    uint32_t _lastStepTime_InUS;

private:
    int32_t _targetPosition_InSteps;
    int32_t _currentPosition_InSteps;

private:
    float _stepsPerRevolution;
    float _nextStepPeriod_InUS;
    float _currentStepPeriod_InUS;
    float _periodOfSlowestStep_InUS;
    float _desiredPeriod_InUSPerStep;
    float _acceleration_InStepsPerSecond;
    float _deceleration_InStepsPerSecond;
    float _desiredSpeed_InStepsPerSecond;
    float _acceleration_InStepsPerUSPerUS;
    float _deceleration_InStepsPerUSPerUS;
    float _minimumPeriodForAStoppedMotion;

private:
    TaskHandle_t _handle = nullptr;
};
