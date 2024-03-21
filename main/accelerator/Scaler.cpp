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

#include "accelerator/Scaler.hpp"

#include <cassert>

namespace accelerator
{

/**
 * Method for calculating the scaling factor
 *
 *                   x_max - scaling_base
 * scaling_factor = ----------------------
 *                          x_max
 *
 * @param minimalValue Minimal value in range
 * @param maximalValue Maximal value in range
 * @return [float] Scaling factor
 */
constexpr float calculateScalingFactor(uint32_t const minimalValue, uint32_t const maximalValue)
{
    return static_cast<float>(maximalValue - minimalValue) / static_cast<float>(maximalValue);
}

/**
 * Function for scaling values by scaling base and scaling factor
 *
 * scaled_x = scaling_base + (scaling_factor * x)
 *
 * @param value Scalable value
 * @param scalingBase Base of scaling
 * @param scalingFactor Scaling factor
 * @return [uint32_t] Scaled value
 */
constexpr uint32_t calculateScaledValue(uint32_t const value, uint32_t const scalingBase, float const scalingFactor)
{
    return scalingBase + static_cast<uint32_t>(static_cast<float>(value) * scalingFactor);
}

Scaler::Scaler(uint32_t const minimalValue, uint32_t const maximalValue) :
    m_scalingFactor(calculateScalingFactor(minimalValue, maximalValue)), m_scalingBase(minimalValue), m_currentValue(0),
    m_maximalValue(maximalValue), m_minimalValue(minimalValue)
{
    assert(minimalValue < maximalValue);
}

void Scaler::registerChangeValueCallback(ChangeValueCallbackFunction const& changeValueCallbackFunction)
{
    m_changeValueCallbackFunction = changeValueCallbackFunction;
}

void Scaler::setValue(uint32_t const value)
{
    m_currentValue = value;

    if (m_currentValue < m_minimalValue)
    {
        m_currentValue = m_minimalValue;
    }

    if (m_currentValue > m_maximalValue)
    {
        m_currentValue = m_maximalValue;
    }

    calculateValue();
}

void Scaler::enable()
{
    m_scalingBase = m_currentValue;
    m_scalingFactor = calculateScalingFactor(m_scalingBase, m_maximalValue);
}

void Scaler::disable()
{
    m_scalingBase = m_minimalValue;
    m_scalingFactor = calculateScalingFactor(m_scalingBase, m_maximalValue);

    calculateValue();
}

void Scaler::calculateValue()
{
    auto scaledValue = calculateScaledValue(m_currentValue, m_scalingBase, m_scalingFactor);

    if (m_changeValueCallbackFunction)
    {
        m_changeValueCallbackFunction(scaledValue);
    }
}

} // namespace accelerator
