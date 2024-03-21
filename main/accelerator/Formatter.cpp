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
// Created by jadjer on 27.09.23.
//

#include "accelerator/Formatter.hpp"

namespace accelerator
{

/**
 * Function to format from one value range to other value range
 *
 * scaled_x = scaling_base + (scaling_factor * x)
 *
 * @param value Scalable value
 * @param scalingBase Base of scaling
 * @param scalingFactor Scaling factor
 * @return [uint32_t] Scaled value
 */
constexpr uint32_t calculateFormattedValue(
    uint32_t const value,
    uint32_t const minimalValueFrom,
    uint32_t const maximalValueFrom,
    uint32_t const minimalValueTo,
    uint32_t const maximalValueTo)
{

    auto a = (value - minimalValueFrom) * (maximalValueTo - minimalValueTo);
    auto b = (maximalValueFrom - minimalValueFrom);
    auto formattedValue = (a / b) + minimalValueTo;

    return formattedValue;
}


Formatter::Formatter(uint32_t minimalValueFrom, uint32_t maximalValueFrom, uint32_t minimalValueTo, uint32_t maximalValueTo) :
    m_sourceRange(minimalValueFrom, maximalValueFrom), m_targetRange(minimalValueTo, maximalValueTo)
{
}

void Formatter::registerChangeValueCallback(ChangeValueCallbackFunction const& changeValueCallbackFunction)
{
    m_changeValueCallbackFunction = changeValueCallbackFunction;
}

void Formatter::setValue(uint32_t value)
{
    if (value < m_sourceRange.minimalValue)
    {
        value = m_sourceRange.minimalValue;
    }

    if (value > m_sourceRange.maximalValue)
    {
        value = m_sourceRange.maximalValue;
    }

    calculateValue(value);
}

void Formatter::calculateValue(uint32_t const value)
{
    auto scaledValue = calculateFormattedValue(
        value, m_sourceRange.minimalValue, m_sourceRange.maximalValue, m_targetRange.minimalValue, m_targetRange.maximalValue);

    if (m_changeValueCallbackFunction)
    {
        m_changeValueCallbackFunction(scaledValue);
    }
}

} // namespace accelerator
