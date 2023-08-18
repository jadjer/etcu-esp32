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

#pragma once

#include <esp_err.h>
#include <mutex>
#include <optional>
#include <string>

#include <ecu/IProtocol.hpp>
#include <ecu/CommandResult.hpp>
#include <ecu/data/EngineData.hpp>
#include <ecu/data/SensorsData.hpp>
#include <ecu/data/VehicleData.hpp>

/**
 * @class ECU
 */
class ElectronicControlUnit {
public:
    /**
     *
     * @param protocolPtr
     */
    explicit ElectronicControlUnit(IProtocolPtr protocolPtr);
    /**
     * Default destructor
     */
    ~ElectronicControlUnit();

public:
    /**
   * @brief
   */
    void detectAllTables();

    /**
   * @brief
   */
    void updateAllData();

public:
    /**
     * @brief
     * @return
     */
    [[nodiscard]] VehicleData getVehicleData() const;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] EngineData getEngineData() const;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] SensorsData getSensorsData() const;

public:
    /**
     *
     */
    void process();

private:
    /**
     *
     * @param table
     * @return
     */
    CommandResultPtr updateDataFromTable(uint8_t table);

private:
    /**
     *
     * @return
     */
    esp_err_t updateDataFromTable10();
    /**
     *
     * @return
     */
    esp_err_t updateDataFromTable11();
    /**
     *
     * @return
     */
    esp_err_t updateDataFromTableD1();

private:
    std::mutex _mutex;
    IProtocolPtr _protocolPtr;

private:
    VehicleData _vehicleData;
    EngineData _engineData;
    SensorsData _sensorsData;
};
