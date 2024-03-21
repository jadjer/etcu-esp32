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

#pragma once

#include <cassert>
#include <cstdint>
#include <functional>

namespace accelerator
{

using ChangeValueCallbackFunction = std::function<void(uint32_t)>;

struct ValueRange
{
    uint32_t minimalValue;
    uint32_t maximalValue;

    ValueRange(uint32_t minimal, uint32_t maximal)
    {
        assert(minimal < maximal);

        minimalValue = minimal;
        maximalValue = maximal;
    }
};

/**
 * @class Formatter
 */
class Formatter
{
public:
    Formatter(uint32_t minimalValueFrom, uint32_t maximalValueFrom, uint32_t minimalValueTo, uint32_t maximalValueTo);

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

private:
    /**
     * Scaling value and call callback functor
     */
    void calculateValue(uint32_t value);

private:
    ChangeValueCallbackFunction m_changeValueCallbackFunction = nullptr;

private:
    ValueRange m_sourceRange;
    ValueRange m_targetRange;
};

} // namespace accelerator
