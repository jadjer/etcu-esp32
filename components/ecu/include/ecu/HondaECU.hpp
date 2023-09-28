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
// Created by jadjer on 28.08.23.
//

#pragma once

#include <shared_mutex>

#include "executor/interface/Node.hpp"
#include "ecu/interface/KLineNetworkConnector.hpp"

/**
 * @namespace ECU
 */
namespace ecu
{

/**
 * @class HondaECU
 */
class HondaECU : public executor::interface::Node
{
public:
    explicit HondaECU(interface::KLineNetworkConnectorPtr networkConnectorPtr);

public:
    [[nodiscard]] uint16_t getRevolutionPerMinute() const;
    [[nodiscard]] uint16_t getSpeedInKilometerPerHour() const;
    [[nodiscard]] uint8_t getThrottlePositionInPercent() const;

protected:
    void process() override;

private:
    void updateSpeedData();
    void updateRevolutePerMinuteData();

private:
    mutable std::shared_mutex m_mutex;
    interface::KLineNetworkConnectorPtr m_networkConnectorPtr;

private:
    uint8_t m_throttlePosition_InPercent;
    uint16_t m_speed_InKilometersPerHour;
    uint16_t m_revolutionPerMinute;
};

} // namespace Ecu
