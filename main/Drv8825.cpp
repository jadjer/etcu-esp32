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

#include "Drv8825.hpp"

#include <cassert>

#include "gpio/Pin.hpp"

DRV8825::DRV8825() :
    m_homePin(std::make_unique<gpio::Pin>(19, gpio::PIN_LEVEL_HIGH)),
    m_stepPin(std::make_unique<gpio::Pin>(11, gpio::PIN_LEVEL_LOW)),
    m_decayPin(std::make_unique<gpio::Pin>(14, gpio::PIN_LEVEL_HIGH)),
    m_faultPin(std::make_unique<gpio::Pin>(21, gpio::PIN_LEVEL_HIGH)),
    m_mode0Pin(std::make_unique<gpio::Pin>(10, gpio::PIN_LEVEL_LOW)),
    m_mode1Pin(std::make_unique<gpio::Pin>(9, gpio::PIN_LEVEL_LOW)),
    m_mode2Pin(std::make_unique<gpio::Pin>(20, gpio::PIN_LEVEL_LOW)),
    m_resetPin(std::make_unique<gpio::Pin>(48, gpio::PIN_LEVEL_HIGH)),
    m_sleepPin(std::make_unique<gpio::Pin>(47, gpio::PIN_LEVEL_HIGH)),
    m_enablePin(std::make_unique<gpio::Pin>(12, gpio::PIN_LEVEL_LOW)),
    m_directionPin(std::make_unique<gpio::Pin>(13, gpio::PIN_LEVEL_LOW))
{
    assert(motor::driver::MOTOR_MICRO_STEPS_COUNT == 8);
    assert(motor::driver::MOTOR_ROTATE_COUNT == 2);
}

void DRV8825::setMicroSteps(motor::driver::MotorMicroSteps microSteps)
{
    if (microSteps == motor::driver::MOTOR_FULL_STEP)
    {
        m_mode0Pin->setLevel(gpio::PIN_LEVEL_LOW);
        m_mode1Pin->setLevel(gpio::PIN_LEVEL_LOW);
        m_mode2Pin->setLevel(gpio::PIN_LEVEL_LOW);
    }

    if (microSteps == motor::driver::MOTOR_2_MICRO_STEPS)
    {
        m_mode0Pin->setLevel(gpio::PIN_LEVEL_HIGH);
        m_mode1Pin->setLevel(gpio::PIN_LEVEL_LOW);
        m_mode2Pin->setLevel(gpio::PIN_LEVEL_LOW);
    }

    if (microSteps == motor::driver::MOTOR_4_MICRO_STEPS)
    {
        m_mode0Pin->setLevel(gpio::PIN_LEVEL_LOW);
        m_mode1Pin->setLevel(gpio::PIN_LEVEL_HIGH);
        m_mode2Pin->setLevel(gpio::PIN_LEVEL_LOW);
    }

    if (microSteps == motor::driver::MOTOR_8_MICRO_STEPS)
    {
        m_mode0Pin->setLevel(gpio::PIN_LEVEL_HIGH);
        m_mode1Pin->setLevel(gpio::PIN_LEVEL_HIGH);
        m_mode2Pin->setLevel(gpio::PIN_LEVEL_LOW);
    }

    if (microSteps == motor::driver::MOTOR_16_MICRO_STEPS)
    {
        m_mode0Pin->setLevel(gpio::PIN_LEVEL_LOW);
        m_mode1Pin->setLevel(gpio::PIN_LEVEL_LOW);
        m_mode2Pin->setLevel(gpio::PIN_LEVEL_HIGH);
    }

    if (microSteps == motor::driver::MOTOR_32_MICRO_STEPS)
    {
        m_mode0Pin->setLevel(gpio::PIN_LEVEL_HIGH);
        m_mode1Pin->setLevel(gpio::PIN_LEVEL_HIGH);
        m_mode2Pin->setLevel(gpio::PIN_LEVEL_HIGH);
    }
}

void DRV8825::setDirection(motor::driver::MotorRotateDirection direction) {
    if (direction == motor::driver::MOTOR_ROTATE_CLOCKWISE)
    {
        m_directionPin->setLevel(gpio::PIN_LEVEL_HIGH);
    }

    if (direction == motor::driver::MOTOR_ROTATE_OTHERWISE)
    {
        m_directionPin->setLevel(gpio::PIN_LEVEL_LOW);
    }
}

void DRV8825::setStepToLow() {
    m_stepPin->setLevel(gpio::PIN_LEVEL_LOW);
}

void DRV8825::setStepToHigh() {
    m_stepPin->setLevel(gpio::PIN_LEVEL_HIGH);
}
