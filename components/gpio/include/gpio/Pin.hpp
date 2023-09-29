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
// Created by jadjer on 30.08.22.
//

#pragma once

#include <cstdlib>

#include "gpio/interface/IPin.hpp"

namespace gpio
{

/**
 * @class Pin
 * @brief
 */
class Pin : public interface::IPin
{
public:
    explicit Pin(uint8_t numberOfPin, bool reverseValue = false);

public:
    [[nodiscard]] PinLevel getLevel() const override;
    void setLevel(PinLevel value) override;

private:
    bool const m_reverseValue;
    uint8_t const m_numberOfPin;
};

} // namespace gpio
