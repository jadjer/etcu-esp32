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
// Created by jadjer on 24.08.22.
//

#pragma once

#include <mutex>
#include <ecu/IProtocol.hpp>

/**
 * @class Protocol
 */
class UartProtocol : public IProtocol {
public:
    /**
     *
     * @param rxPin
     * @param txPin
     */
    UartProtocol(uint8_t rxPinNum, uint8_t txPinNum);

    /**
     * Default destructor
     */
    ~UartProtocol() override;

public:
    /**
     * @brief
     * @return
     */
    esp_err_t connect() override;

private:
    /**
     * ECU wake up
     */
    esp_err_t wakeUp() const;

    /**
     * ECU init
     * @return
     */
    esp_err_t initialize();

public:
    [[nodiscard]] bool isConnected() const override;

public:
    /**
     * @brief
     * @param data
     * @param len
     */
    esp_err_t writeData(uint8_t const *data, size_t len) override;

    /**
     * @brief
     * @return
     */
    CommandResultPtr readData() override;

private:
    /**
     *
     * @param len
     */
    void waitDataFromUart(size_t len) const;

    /**
     *
     * @return
     */
    [[nodiscard]] size_t getBufferSize() const;

private:
    std::mutex _mutex;

private:
    bool _isConnected;
    uint8_t _rxPinNum;
    uint8_t _txPinNum;
    uint8_t _uartPortNum;
};
