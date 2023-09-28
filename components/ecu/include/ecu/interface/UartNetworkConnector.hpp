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

#pragma once

#include <memory>

#include "ecu/interface/NetworkConnector.hpp"

/**
 * @namespace ecu::interface
 */
namespace ecu::interface
{

/**
 * @interface INetworkConnector
 */
class UartNetworkConnector : public NetworkConnector
{

public:
    /**
     * Read one byte from network channel
     * @return CommandResult struct
     */
    virtual Byte readByte() = 0;
    /**
     * Read data from network channel
     * @param requiredLength Count of bytes
     * @return
     */
    virtual Bytes readBytes(size_t requiredLength) = 0;
    /**
     * Write data to network channel
     * @param data Bytes array
     */
    virtual void write(Bytes const& data) = 0;
};

using UartNetworkConnectorPtr = std::unique_ptr<UartNetworkConnector>;

} // namespace ecu::interface
