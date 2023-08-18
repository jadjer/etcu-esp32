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
// Created by jadjer on 01.09.22.
//

#include "ecu/BaseElectronicControlUnit.hpp"
#include <ecu/CommandResult.hpp>
#include <ecu/UartProtocol.hpp>
#include "utils.hpp"

BaseElectronicControlUnit::BaseElectronicControlUnit(IProtocolPtr protocolPtr) :
        _protocolPtr(std::move(protocolPtr)) {
}

BaseElectronicControlUnit::~BaseElectronicControlUnit() = default;

CommandResultPtr BaseElectronicControlUnit::getDataFromTable(uint8_t tableAddress) {
    uint8_t messageLength = 5;

    uint8_t message[] = {REQUEST_TYPE, messageLength, REQUEST_ALL_DATA_IN_TABLE, tableAddress, 0};
    message[4] = calcChecksum(message, messageLength);

    _protocolPtr->writeData(message, messageLength);

    return _protocolPtr->readData();
}

CommandResultPtr BaseElectronicControlUnit::getPartialDataFromTable(uint8_t tableAddress, uint8_t beginSlice, uint8_t endSlice) {
    uint8_t messageLength = 7;

    uint8_t message[] = {REQUEST_TYPE, messageLength, REQUEST_PARTIAL_DATA_IN_TABLE, tableAddress, beginSlice, endSlice, 0};
    message[6] = calcChecksum(message, messageLength);

    _protocolPtr->writeData(message, messageLength);

    return _protocolPtr->readData();
}
