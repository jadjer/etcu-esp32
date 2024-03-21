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
// Created by jadjer on 23.09.22.
//

#pragma once

#include <thread>

#include "executor/Node.hpp"
#include "gpio/interface/IPin.hpp"

namespace indicator
{

/**
 * @brief
 */
class Indicator : public executor::Node
{
public:
    explicit Indicator(uint8_t pinNum);

    ~Indicator() override;

public:
    /**
     * @brief
     */
    virtual void enable();

    /**
     * @brief
     */
    virtual void disable();

    /**
     * @brief
     * @param delayMs
     */
    virtual void blink(int delayMs);

protected:
    bool m_enableFlag;
    uint8_t m_taskValue;
    std::thread m_thread;
    std::unique_ptr<gpio::interface::IPin> m_indicatorPin;

protected:
    /**
     * @brief
     * @param delayMs
     */
    virtual void blinkTask() = 0;

    void process() override;
};

} // namespace indicator
