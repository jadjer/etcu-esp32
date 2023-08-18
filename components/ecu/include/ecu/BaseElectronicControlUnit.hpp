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

enum {
    REQUEST_TYPE = 0x72,
    REQUEST_ALL_DATA_IN_TABLE = 0x71,
    REQUEST_PARTIAL_DATA_IN_TABLE = 0x72,
    REQUEST_UNKNOWN_1_ = 0x73,
    REQUEST_UNKNOWN_2 = 0x74,
};

/**
 * @class ECU
 */
class BaseElectronicControlUnit {
public:
    /**
     *
     * @param protocolPtr
     */
    explicit BaseElectronicControlUnit(IProtocolPtr protocolPtr);

    /**
     * Default destructor
     */
    virtual ~BaseElectronicControlUnit();

public:
    /**
     *
     */
    virtual void process() = 0;

protected:
    /**
     *
     * @param tableAddress
     * @return
     */
    CommandResultPtr getDataFromTable(uint8_t tableAddress);

    /**
     *
     * @param tableAddress
     * @param beginSlice
     * @param endSlice
     * @return
     */
    CommandResultPtr getPartialDataFromTable(uint8_t tableAddress, uint8_t beginSlice, uint8_t endSlice);

protected:
    IProtocolPtr _protocolPtr;
};
