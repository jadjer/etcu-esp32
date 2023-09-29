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
// Created by jadjer on 15.08.23.
//

#pragma once

#include <cstdint>

/**
 * @namespace Executor
 */
namespace executor
{

class Executor;

/**
 * @interface Node
 * Abstract class for node object
 */
class Node
{
    friend Executor;

public:
    explicit Node(float frequency = 100);
    /**
     * Virtual default destructor
     */
    virtual ~Node() = default;

public:
    /**
     * Set frequency for update node
     * @param frequency
     */
    void setFrequency(float frequency);

protected:
    /**
     * Service's method for call process() with settled frequency
     */
    void spinOnce();

private:
    /**
     * Business data implementation
     */
    virtual void process() = 0;

private:
    uint32_t m_updatePeriod_InUS;
    uint32_t m_lastSpinTime_InUS;
};

} // namespace executor

#include <memory>

using NodePtr = std::shared_ptr<executor::Node>;
