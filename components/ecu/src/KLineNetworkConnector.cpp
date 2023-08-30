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

#include "ECU/KLineNetworkConnector.hpp"
#include "ECU/exception.hpp"

#include <thread>
#include <driver/gpio.h>

namespace ECU
{

void configurePin(Byte const numberOfPin)
{
    gpio_config_t buttonConfig = {
        .pin_bit_mask = (1ull << numberOfPin),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_ENABLE,
        .intr_type = GPIO_INTR_DISABLE};

    auto status = gpio_config(&buttonConfig);
    auto errorCode = ErrorCode::EspStatusCodeToErrorCode(status);
    if (errorCode != ErrorCode::Enum::Success)
    {
        throw ECU::NetworkException("qwe", errorCode);
    }
}

void setPin(Byte const numberOfPin, Byte const value)
{
    auto status = gpio_set_level(static_cast<gpio_num_t>(numberOfPin), value);
    auto errorCode = ErrorCode::EspStatusCodeToErrorCode(status);
    if (errorCode != ErrorCode::Enum::Success)
    {
        throw ECU::NetworkException("qwe", errorCode);
    }
}

uint16_t calculateCheckSum(Byte resultCode, Byte dataLength, Byte queryType, Bytes const& payload, Byte checkSum)
{
    uint16_t calculatedCheckSum = 0;

    checkSum -= resultCode;
    checkSum -= dataLength;
    checkSum -= queryType;
    checkSum -= checkSum;

    for (Byte const i : payload)
    {
        checkSum -= i;
    }

    return calculatedCheckSum;
}

KLineNetworkConnector::KLineNetworkConnector(Byte const numberOfTxPin, Interface::UartNetworkConnectorPtr networkConnectorPtr) :
    m_networkConnectorPtr(std::move(networkConnectorPtr)), m_isConnected(false)
{
    configurePin(numberOfTxPin);

    setPin(numberOfTxPin, 0);
    std::this_thread::sleep_for(std::chrono::milliseconds(70));

    setPin(numberOfTxPin, 1);
    std::this_thread::sleep_for(std::chrono::milliseconds(120));
}

void KLineNetworkConnector::connect()
{
    m_networkConnectorPtr->connect();
    m_isConnected = true;
}

Bytes KLineNetworkConnector::readData()
{
    if (not m_isConnected)
    {
        return {};
    }

    auto const minimalDataLength = 4;

    Byte resultCode = m_networkConnectorPtr->readByte();
    if (resultCode != 0x02)
    {
        // TODO KLine result code exception
    }

    Byte dataLength = m_networkConnectorPtr->readByte();
    if (dataLength < minimalDataLength)
    {
        // TODO KLine data length exception
    }

    Byte queryType = m_networkConnectorPtr->readByte();
    if ((queryType != 0x71) or (queryType != 0x72)) {
        // TODO KLine invalid query type exception
    }

    Bytes payload = m_networkConnectorPtr->readBytes(dataLength - minimalDataLength);

    Byte checkSum = m_networkConnectorPtr->readByte();
    auto calculatedCheckSum = calculateCheckSum(resultCode, dataLength, queryType, payload, checkSum);
    if (checkSum != calculatedCheckSum) {
        // TODO KLine invalid data exception
    }

    return payload;
}

void KLineNetworkConnector::writeData(Bytes const& data)
{
    if (not m_isConnected)
    {
        return;
    }

    m_networkConnectorPtr->write(data);
}

} // namespace ECU
