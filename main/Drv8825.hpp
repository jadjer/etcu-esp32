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

#include <memory>

#include "motor/driver/interface/IDriver.hpp"
#include "gpio/interface/IPin.hpp"

class DRV8825 : public motor::driver::interface::IDriver
{
public:
    DRV8825();
    ~DRV8825() override = default;

public:
    void setMicroSteps(motor::driver::MotorMicroSteps microSteps) override;

public:
    void setDirection(motor::driver::MotorRotateDirection direction) override;

public:
    void setStepToLow() override;
    void setStepToHigh() override;

private:
    std::unique_ptr<gpio::interface::IPin> m_homePin;
    std::unique_ptr<gpio::interface::IPin> m_stepPin;
    std::unique_ptr<gpio::interface::IPin> m_decayPin;
    std::unique_ptr<gpio::interface::IPin> m_faultPin;
    std::unique_ptr<gpio::interface::IPin> m_mode0Pin;
    std::unique_ptr<gpio::interface::IPin> m_mode1Pin;
    std::unique_ptr<gpio::interface::IPin> m_mode2Pin;
    std::unique_ptr<gpio::interface::IPin> m_resetPin;
    std::unique_ptr<gpio::interface::IPin> m_sleepPin;
    std::unique_ptr<gpio::interface::IPin> m_enablePin;
    std::unique_ptr<gpio::interface::IPin> m_directionPin;
};
