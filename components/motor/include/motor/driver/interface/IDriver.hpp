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

//
// Created by jadjer on 29.09.23.
//

#pragma once

#include <cstdint>

namespace motor::driver
{

enum MotorMicroSteps {
    MOTOR_FULL_STEP = 0,
    MOTOR_2_MICRO_STEPS,
    MOTOR_4_MICRO_STEPS,
    MOTOR_8_MICRO_STEPS,
    MOTOR_16_MICRO_STEPS,
    MOTOR_32_MICRO_STEPS,
    MOTOR_64_MICRO_STEPS,
    MOTOR_128_MICRO_STEPS,
    MOTOR_MICRO_STEPS_COUNT = 8
};

enum MotorRotateDirection
{
    MOTOR_ROTATE_CLOCKWISE = 0,
    MOTOR_ROTATE_OTHERWISE,
    MOTOR_ROTATE_COUNT = 2
};

namespace interface
{

class IDriver
{
public:
    virtual ~IDriver() = default;

public:
    virtual void setMicroSteps(MotorMicroSteps) = 0;

public:
    virtual void setDirection(MotorRotateDirection direction) = 0;

public:
    virtual void setStepToLow() = 0;
    virtual void setStepToHigh() = 0;
};

} // namespace interface
} // namespace motor::driver

#include <memory>

using IDriverPtr = std::unique_ptr<motor::driver::interface::IDriver>;
