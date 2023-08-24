//
// Created by Сергей Матышев on 19.08.23.
//

#include "ecu/UartNetworkConnector.h"

#include "NetworkException.h"

#include <driver/gpio.h>
#include <driver/uart.h>

#include <chrono>
#include <cstdint>
#include <thread>
#include <vector>


namespace
{

void WakeUp(uint8_t const numberOfTxPin)
{
    auto errorCode = ESP_OK;

    gpio_config_t buttonConfig = {
        .pin_bit_mask = (1ull << numberOfTxPin),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_ENABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    errorCode = gpio_config(&buttonConfig);

    /*
     * TODO:
     * Нужно добавить фильтрацию ошибок, есть идея как это сделать, отдельный класс, который будет принимать
     * статус и если статус не ок - генерировать нужную ошибку, пока что оставлю так, как заготовку под изменения
     * */

    if (ErrorCode::EspStatusCodeToErrorCode(errorCode) != ErrorCode::Success)
    {
        std::string const gpioConfigSetError = "Error to set gpio config.";
        throw Common::NetworkException(gpioConfigSetError, ErrorCode::EspStatusCodeToErrorCode(errorCode));
    }

    int level = 0;

    errorCode = gpio_set_level(static_cast<gpio_num_t>(numberOfTxPin), level);
    if (errorCode != ESP_OK)
    {
        std::string const gpioConfigSetError = "Error to set gpio level " + std::to_string(level) + " .";
        throw Common::NetworkException(gpioConfigSetError, ErrorCode::EspStatusCodeToErrorCode(errorCode));
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(70));

    level = 1;

    errorCode = gpio_set_level(static_cast<gpio_num_t>(numberOfTxPin), level);

    if (errorCode != ESP_OK)
    {
        std::string const gpioConfigSetError = "Error to set gpio level " + std::to_string(level) + " .";

        throw Common::NetworkException(gpioConfigSetError, ErrorCode::EspStatusCodeToErrorCode(errorCode));
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(120));
}

void WriteDataToUart(std::vector<uint8_t> const& data, uint8_t const numberOfUartPorts)
{
    std::size_t const size = data.size();
    uint8_t* cStyleArray = new uint8_t[size];

    for (size_t i = 0; i < data.size(); ++i)
    {
        cStyleArray[i] = data[i];
    }

    size_t sendLen = uart_write_bytes(numberOfUartPorts, cStyleArray, size);

    if (sendLen != size)
    {
        std::string const errorMessage = "Data write error, abort write to uart operation";
        throw Common::UartWriteError(errorMessage, ErrorCode::OperationAborted);
    }
}

} // namespace

namespace NetworkConnector
{

UartNetworkConnector::UartNetworkConnector(uint8_t numberOfRxPin, uint8_t numberOfTxPin, uint8_t numberOfUartPorts) :
    m_numberOfRxPin(numberOfRxPin), m_numberOfTxPin(numberOfTxPin), m_numberOfUartPorts(numberOfUartPorts)
{
    std::vector<uint8_t> const wakeUpMessage = {0xFE, 0x04, 0xFF, 0xFF};
    std::vector<uint8_t> const initializeMessage = {0x72, 0x05, 0x00, 0xF0, 0x99};
}

CommandResultPtr UartNetworkConnector::Read() {}

void UartNetworkConnector::Write(std::vector<uint8_t> const& data)
{
    std::lock_guard<std::mutex> lockGuard(m_mutex);


    /**
     * Input buffer clear
     */
    auto const status = uart_flush(m_numberOfUartPorts);

    auto const errorCode = ErrorCode::EspStatusCodeToErrorCode(status);

    /*
     * TODO:
     * Нужно добавить фильтрацию ошибок, есть идея как это сделать, отдельный класс, который будет принимать
     * статус и если статус не ок - генерировать нужную ошибку, пока что оставлю так, как заготовку под изменения
     * */

    if (errorCode != ErrorCode::Enum::Success)
    {
        auto const errorMessage = "Write error";
        throw Common::NetworkException(errorMessage, errorCode);
    }

    WriteDataToUart(data, m_numberOfUartPorts);
}

void UartNetworkConnector::Connect()
{
    auto errorCode = ESP_OK;

    WakeUp(m_numberOfUartPorts);
}

} // namespace NetworkConnector