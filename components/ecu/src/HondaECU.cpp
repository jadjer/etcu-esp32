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

#include "ECU/HondaECU.hpp"

#include <chrono>
#include <thread>

namespace ECU
{

HondaECU::HondaECU(Interface::KLineNetworkConnectorPtr networkConnectorPtr) :
    m_networkConnectorPtr(std::move(networkConnectorPtr)),
    m_throttlePosition_InPercent(0),
    m_speed_InKilometersPerHour(0),
    m_revolutionPerMinute(0)
{
    Bytes const wakeUpMessage = {0xFE, 0x04, 0xFF, 0xFF};
    Bytes const initializeMessage = {0x72, 0x05, 0x00, 0xF0, 0x99};
    auto const delayBetweenInitCommands = std::chrono::milliseconds(200);

    m_networkConnectorPtr->connect();

    m_networkConnectorPtr->writeData(wakeUpMessage);
    std::this_thread::sleep_for(delayBetweenInitCommands);

    m_networkConnectorPtr->writeData(initializeMessage);
    std::this_thread::sleep_for(delayBetweenInitCommands);
}

uint16_t HondaECU::getSpeedInKilometerPerHour() const
{
    std::shared_lock lock(m_mutex);
    return m_speed_InKilometersPerHour;
}

uint16_t HondaECU::getRevolutionPerMinute() const
{
    std::shared_lock lock(m_mutex);
    return m_revolutionPerMinute;
}

uint8_t HondaECU::getThrottlePosition_InPercent() const
{
    return m_throttlePosition_InPercent;
}

void HondaECU::spinOnce() {
    updateAllSensorsData();
}

void HondaECU::updateAllSensorsData() {
    updateSpeedData();
}

void HondaECU::updateSpeedData() {
    std::unique_lock lock(m_mutex);

    Bytes const speedSensorDataRequest = {0x72, 0x07, 0x72, 0x11, 0x13, 0x14, 0x00};

    m_networkConnectorPtr->writeData(speedSensorDataRequest);
    Bytes payload = m_networkConnectorPtr->readData();

    // TODO Speed data from payload value
}

void HondaECU::updateRevolutePerMinuteData() {
    std::unique_lock lock(m_mutex);

    Bytes const rpmSensorDataRequest = {0x72, 0x07, 0x72, 0x11, 0x00, 0x01, 0x00};

    m_networkConnectorPtr->writeData(rpmSensorDataRequest);
    Bytes payload = m_networkConnectorPtr->readData();

    // TODO RPM data from payload value
}

} // namespace Ecu
