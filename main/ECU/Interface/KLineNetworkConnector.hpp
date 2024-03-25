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
// Created by jadjer on 29.08.23.
//

#pragma once

#include <memory>

#include "ECU/Interface/NetworkConnector.hpp"

/**
 * @namespace ECU::Interface
 */
namespace ECU::Interface
{

/**
 * @interface INetworkConnector
 */
class KLineNetworkConnector : public NetworkConnector
{
public:
    /**
     * Read data from network channel
     * @return CommandResult struct
     */
    virtual Bytes readData() = 0;
    /**
     * Write data to network channel
     * @param data Bytes array
     */
    virtual void writeData(Bytes const& data) = 0;
};

using KLineNetworkConnectorPtr = std::unique_ptr<KLineNetworkConnector>;

} // namespace ECU::Interface
