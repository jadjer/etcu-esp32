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

#include "ecu/ElectronicControlUnit.hpp"
#include <ecu/CommandResult.hpp>
#include <ecu/UartProtocol.hpp>
#include "utils.hpp"

ElectronicControlUnit::ElectronicControlUnit(IProtocolPtr protocolPtr) :
        _mutex(),
        _protocolPtr(std::move(protocolPtr)),
        _vehicleData({.batteryVolts = 0.0, .speed = 0, .state = 0}),
        _engineData({.rpm = 0, .fuelInject = 0, .ignitionAdvance = 0}),
        _sensorsData({.tpsPercent = 0.0, .tpsVolts = 0.0, .ectTemp = 0, .ectVolts = 0.0, .iatTemp = 0, .iatVolts = 0.0, .mapPressure = 0, .mapVolts = 0.0}) {
}

ElectronicControlUnit::~ElectronicControlUnit() = default;

void ElectronicControlUnit::detectAllTables() {
    if (not _protocolPtr->isConnected()) {
        return;
    }

    std::lock_guard<std::mutex> lock(_mutex);

    for (uint8_t address = 0x0; address < 0xff; address++) {
        uint8_t message[5] = {0x72, 0x05, 0x71, address, 0x00};
        message[4] = calcChecksum(message, 4);

        _protocolPtr->writeData(message, 5);
        auto result = _protocolPtr->readData();
    }
}

void ElectronicControlUnit::updateAllData() {
    if (not _protocolPtr->isConnected()) {
        return;
    }

    std::lock_guard<std::mutex> lock(_mutex);

    updateDataFromTable10();
    updateDataFromTable11();
    updateDataFromTableD1();
}

VehicleData ElectronicControlUnit::getVehicleData() const {
    return _vehicleData;
}

EngineData ElectronicControlUnit::getEngineData() const {
    return _engineData;
}

SensorsData ElectronicControlUnit::getSensorsData() const {
    return _sensorsData;
}

void ElectronicControlUnit::process() {
    if (not _protocolPtr->isConnected()) {
        return;
    }
}

CommandResultPtr ElectronicControlUnit::updateDataFromTable(uint8_t table) {
    uint8_t message[5] = {0x72, 0x05, 0x71, table, 0x00};
    message[4] = calcChecksum(message, 4);

    _protocolPtr->writeData(message, 5);

    return _protocolPtr->readData();
}

esp_err_t ElectronicControlUnit::updateDataFromTable10() {
    auto result = updateDataFromTable(0x10);
    if (result == nullptr) {
        return ESP_FAIL;
    }

    if (result->length >= 0x16) {
        _engineData.rpm = (result->data[4] << 8) + result->data[5];
        _sensorsData.tpsVolts = calcValueDivide256(result->data[6]);
        _sensorsData.tpsPercent = calcValueDivide16(result->data[7]);
        _sensorsData.ectVolts = calcValueDivide256(result->data[8]);
        _sensorsData.ectTemp = calcValueMinus40(result->data[9]);
        _sensorsData.iatVolts = calcValueDivide256(result->data[10]);
        _sensorsData.iatTemp = calcValueMinus40(result->data[11]);
        _sensorsData.mapVolts = calcValueDivide256(result->data[12]);
        _sensorsData.mapPressure = result->data[13];
        _vehicleData.batteryVolts = calcValueDivide10(result->data[16]);
        _vehicleData.speed = result->data[17];
        _engineData.fuelInject = (result->data[18] << 8) + result->data[19];
        _engineData.ignitionAdvance = result->data[20];
    }

    return ESP_OK;
}

esp_err_t ElectronicControlUnit::updateDataFromTable11() {
    auto result = updateDataFromTable(0x11);
    if (result == nullptr) {
        return ESP_FAIL;
    }

    if (result->length >= 0x19) {
        _engineData.rpm = (result->data[4] << 8) + result->data[5];
        _sensorsData.tpsVolts = calcValueDivide256(result->data[6]);
        _sensorsData.tpsPercent = calcValueDivide16(result->data[7]);
        _sensorsData.ectVolts = calcValueDivide256(result->data[8]);
        _sensorsData.ectTemp = calcValueMinus40(result->data[9]);
        _sensorsData.iatVolts = calcValueDivide256(result->data[10]);
        _sensorsData.iatTemp = calcValueMinus40(result->data[11]);
        _sensorsData.mapVolts = calcValueDivide256(result->data[12]);
        _sensorsData.mapPressure = result->data[13];
        _vehicleData.batteryVolts = calcValueDivide10(result->data[16]);
        _vehicleData.speed = result->data[17];
        _engineData.fuelInject = (result->data[18] << 8) + result->data[19];
        _engineData.ignitionAdvance = result->data[20];
    }

    return ESP_OK;
}

esp_err_t ElectronicControlUnit::updateDataFromTableD1() {
    auto result = updateDataFromTable(0xd1);
    if (result == nullptr) {
        return ESP_FAIL;
    }

    if (result->length >= 0xb) {
        _vehicleData.state = result->data[4];
    }

    return ESP_OK;
}
