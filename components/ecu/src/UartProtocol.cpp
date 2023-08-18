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

#include <ecu/UartProtocol.hpp>
#include <driver/gpio.h>
#include <driver/uart.h>
#include <thread>
#include "utils.hpp"

static const size_t BufferSize = 1024;

UartProtocol::UartProtocol(uint8_t rxPinNum, uint8_t txPinNum) :
        _mutex(),
        _isConnected(false),
        _rxPinNum(rxPinNum),
        _txPinNum(txPinNum),
        _uartPortNum(0) {
}

UartProtocol::~UartProtocol() = default;

esp_err_t UartProtocol::connect() {
    auto errorCode = ESP_OK;

    errorCode = wakeUp();
    if (errorCode != ESP_OK) {
        return errorCode;
    }

    errorCode = initialize();
    if (errorCode != ESP_OK) {
        return errorCode;
    }

    _isConnected = true;

    return errorCode;
}

esp_err_t UartProtocol::wakeUp() const {
    auto errorCode = ESP_OK;

    gpio_config_t buttonConfig = {
            .pin_bit_mask = (1ull << _txPinNum),
            .mode = GPIO_MODE_OUTPUT,
            .pull_up_en = GPIO_PULLUP_DISABLE,
            .pull_down_en = GPIO_PULLDOWN_ENABLE,
            .intr_type = GPIO_INTR_DISABLE,
    };
    errorCode = gpio_config(&buttonConfig);
    if (errorCode != ESP_OK) {
        return errorCode;
    }

    errorCode = gpio_set_level(static_cast<gpio_num_t>(_txPinNum), 0);
    if (errorCode != ESP_OK) {
        return errorCode;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(70));

    errorCode = gpio_set_level(static_cast<gpio_num_t>(_txPinNum), 1);
    if (errorCode != ESP_OK) {
        return errorCode;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(120));

    return errorCode;
}

esp_err_t UartProtocol::initialize() {
    const uint8_t wakeupMessage[] = {0xFE, 0x04, 0xFF, 0xFF};
    const uint8_t initializeMessage[] = {0x72, 0x05, 0x00, 0xF0, 0x99};

    esp_err_t errorCode = ESP_OK;

    /**
     * Setup UART
     */
    uart_config_t uart_config = {
            .baud_rate = 10400,
            .data_bits = UART_DATA_8_BITS,
            .parity = UART_PARITY_DISABLE,
            .stop_bits = UART_STOP_BITS_1,
            .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
            .rx_flow_ctrl_thresh = 0,
            .source_clk = UART_SCLK_DEFAULT
    };

    errorCode = uart_driver_install(0, BufferSize * 2, BufferSize * 2, 0, nullptr, 0);
    if (errorCode != ESP_OK) {
        return errorCode;
    }

    errorCode = uart_param_config(0, &uart_config);
    if (errorCode != ESP_OK) {
        return errorCode;
    }

    errorCode = uart_set_pin(0, _txPinNum, _rxPinNum, -1, -1);
    if (errorCode != ESP_OK) {
        return errorCode;
    }

    /**
     * Send wakeup message...
     */
    writeData(wakeupMessage, sizeof(wakeupMessage));

    /**
     * Wait
     */
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    /**
     * Send initialize message...
     */
    writeData(initializeMessage, sizeof(initializeMessage));

    /**
     * Read answer from ECU
     */
    auto result = readData();
    if (result == nullptr) {
        return ESP_ERR_INVALID_RESPONSE;
    }

    if (result->code != 0x02) {
        return ESP_ERR_INVALID_RESPONSE;
    }

    if (result->length != 4) {
        return ESP_ERR_INVALID_SIZE;
    }

    if (result->command != 0x00) {
        return ESP_ERR_INVALID_ARG;
    }

    if (result->checksum != 0xfa) {
        return ESP_ERR_INVALID_CRC;
    }

    return errorCode;
}

bool UartProtocol::isConnected() const {
    return _isConnected;
}

esp_err_t UartProtocol::writeData(uint8_t const *data, size_t len) {
    std::lock_guard<std::mutex> lockGuard(_mutex);

    esp_err_t errorCode = ESP_OK;

    /**
     * Input buffer clear
     */
    errorCode = uart_flush(_uartPortNum);
    if (errorCode != ESP_OK) {
        return errorCode;
    }

    /**
     * Data write to uart
     */
    size_t sendLen = uart_write_bytes(_uartPortNum, data, len);
    if (sendLen != len) {
        return ESP_ERR_NOT_FINISHED;
    }

//    //  Wait echo data from ECU
//    waitDataFromUart(len);
//
//    //  Echo data's delete from RX buffer
//    uint8_t buffer[len];
//    uart_read_bytes(_uartPortNum, &buffer, len, 20 / portTICK_PERIOD_MS);

    return errorCode;
}

CommandResultPtr UartProtocol::readData() {
    std::lock_guard<std::mutex> lockGuard(_mutex);

    size_t packageMinimumLength = 4;

    /**
     * Check minimum package size
     */
    if (getBufferSize() < packageMinimumLength) {
        return nullptr;
    }

    auto result = std::make_unique<CommandResult>();

    {
        uint8_t buffer[3];
        size_t len = uart_read_bytes(_uartPortNum, &buffer, 3, 1 / portTICK_PERIOD_MS);
        if (len != 3) {
            /**
             * Error read data from uart
             */
            return nullptr;
        }

        result->code = buffer[0];
        result->length = buffer[1];
        result->command = buffer[2];
    }

    if (result->length < packageMinimumLength) {
        /**
         * The message is very small
         */
        return nullptr;
    }

    if (result->length > SOC_UART_FIFO_LEN) {
        /**
         * The message is too big
         */
        return nullptr;
    }

    result->data = new uint8_t[result->length];
    result->data[0] = result->code;
    result->data[1] = result->length;
    result->data[2] = result->command;

    {
        size_t responsePayloadLength = result->length - 3;

        /**
         * Wait full package from uart
         */
        waitDataFromUart(responsePayloadLength);

        /**
         * Read package data from uart
         */
        size_t len = uart_read_bytes(_uartPortNum, &result->data[3], responsePayloadLength, 100 / portTICK_PERIOD_MS);
        if (len != responsePayloadLength) {
            /**
             * Error read data from uart
             */
            return nullptr;
        }

        result->checksum = result->data[result->length - 1];
    }

    uint8_t calculatedChecksum = calcChecksum(result->data, result->length - 1);
    if (result->checksum != calculatedChecksum) {
        /**
         * The checksum does not match
         */
        return nullptr;
    }

    return result;
}

void UartProtocol::waitDataFromUart(size_t len) const {
    while (getBufferSize() < len) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

size_t UartProtocol::getBufferSize() const {
    size_t bufferLen = 0;

    auto errorCode = uart_get_buffered_data_len(_uartPortNum, &bufferLen);
    if (errorCode != ESP_OK) {
        return 0;
    }

    return bufferLen;
}
