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

#include <memory>
#include <esp_err.h>
#include <ecu/CommandResult.hpp>

/**
 * @class  IProtocol
 */
class IProtocol {
public:
    virtual ~IProtocol() = default;

public:
    /**
     *
     * @return
     */
    virtual esp_err_t connect() = 0;

public:
    /**
     *
     * @return
     */
    [[nodiscard]] virtual bool isConnected() const = 0;

public:
    /**
     *
     * @param data
     * @param len
     * @return
     */
    virtual esp_err_t writeData(uint8_t const *data, size_t len) = 0;
    /**
     *
     * @return
     */
    virtual CommandResultPtr readData() = 0;
};

using IProtocolPtr = std::unique_ptr<IProtocol>;
