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
// Created by jadjer on 26.09.23.
//

#pragma once

#include <cstdint>
#include <functional>

namespace accelerator
{

using ChangeValueCallbackFunction = std::function<void(uint32_t)>;

/**
 * @class Scaler
 */
class Scaler
{
public:
    explicit Scaler(uint32_t minimalValue, uint32_t maximalValue);

public:
    /**
     * Register callback functor
     * @param changeValueCallbackFunction Callable functor
     */
    void registerChangeValueCallback(ChangeValueCallbackFunction const& changeValueCallbackFunction);

public:
    /**
     * Set new value fro scaling
     * @param value Scalable value
     */
    void setValue(uint32_t value);

public:
    /**
     * Enable scaling values
     */
    void enable();

    /**
     * Disable scaling values
     */
    void disable();

private:
    /**
     * Scaling value and call callback functor
     */
    void calculateValue();

private:
    ChangeValueCallbackFunction m_changeValueCallbackFunction = nullptr;

private:
    float m_scalingFactor;
    uint32_t m_scalingBase;
    uint32_t m_currentValue;
    uint32_t m_maximalValue;
    uint32_t m_minimalValue;
};

} // namespace accelerator
