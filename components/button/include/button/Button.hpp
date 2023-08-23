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

/**
 * @class Button
 * @brief
 */
class Button {
public:
    explicit Button(uint8_t pinNum, bool invertedValue = false);

    virtual ~Button();

public:
    /**
     * @brief
     * @return
     */
    [[nodiscard]] bool isPressed() const;

private:
    uint8_t _pinNum;
    bool _invertedValue;
};