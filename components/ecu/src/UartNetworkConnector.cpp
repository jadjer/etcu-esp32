//
// Created by Сергей Матышев on 19.08.23.
//

#include "ECU/UartNetworkConnector.hpp"
#include "ECU/exception.hpp"
#include "Common/Interface/ErrorCode.hpp"

#include <driver/uart.h>
#include <thread>
#include <cmath>

namespace ECU
{

void writeDataToUart(Bytes const& data, Byte const numberOfUartPorts)
{
    std::size_t const size = data.size();
    auto cStyleArray = new Byte[size];

    for (size_t i = 0; i < data.size(); ++i)
    {
        cStyleArray[i] = data[i];
    }

    size_t sendLen = uart_write_bytes(numberOfUartPorts, cStyleArray, size);

    if (sendLen != size)
    {
        std::string const errorMessage = "Data write error, abort write to uart operation";
        throw ECU::UartWriteError(errorMessage, ErrorCode::OperationAborted);
    }
}

constexpr uint16_t calculateDelayForWaitDataFromUartInMilliseconds(size_t const bytes, uint16_t const baudRate) {
    auto const bytePerBaud = 1;
    auto const bytePerSecond = baudRate * bytePerBaud;
    auto const bytesPerMillisecond = bytePerSecond / 1000;

    auto minimalDelay_InUS = 1;

    if (bytes < bytesPerMillisecond) {
        return minimalDelay_InUS;
    }

    auto delay_InUS = (bytes / bytesPerMillisecond) + minimalDelay_InUS;
    return delay_InUS;
}

UartNetworkConnector::UartNetworkConnector(Byte numberOfRxPin, Byte numberOfTxPin, Byte numberOfUartPorts) :
    m_numberOfRxPin(numberOfRxPin), m_numberOfTxPin(numberOfTxPin), m_numberOfUartPorts(numberOfUartPorts), m_baudRate(10400)
{
}

void UartNetworkConnector::connect()
{
    size_t const bufferSize = 1024;

    auto status = uart_driver_install(m_numberOfUartPorts, bufferSize, bufferSize, 0, nullptr, 0);
    auto errorCode = ErrorCode::EspStatusCodeToErrorCode(status);
    if (errorCode != ErrorCode::Enum::Success)
    {
        auto const errorMessage = "Write error";
        throw ECU::NetworkException(errorMessage, errorCode);
    }

    /**
     * Setup UART
     */
    uart_config_t uart_config = {
        .baud_rate = m_baudRate,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .rx_flow_ctrl_thresh = 0,
        .source_clk = UART_SCLK_DEFAULT};

    status = uart_param_config(m_numberOfUartPorts, &uart_config);
    errorCode = ErrorCode::EspStatusCodeToErrorCode(status);
    if (errorCode != ErrorCode::Enum::Success)
    {
        throw ECU::NetworkException("Write error", errorCode);
    }

    status = uart_set_pin(m_numberOfUartPorts, m_numberOfTxPin, m_numberOfRxPin, -1, -1);
    errorCode = ErrorCode::EspStatusCodeToErrorCode(status);
    if (errorCode != ErrorCode::Enum::Success)
    {
        throw ECU::NetworkException("Write error", errorCode);
    }
}

Byte UartNetworkConnector::readByte()
{
    std::lock_guard<std::mutex> lockGuard(m_mutex);

    Byte resultData;

    size_t requiredLength = 1;

    auto delay_InUS = calculateDelayForWaitDataFromUartInMilliseconds(requiredLength, m_baudRate);
    auto resultLength = uart_read_bytes(m_numberOfUartPorts, &resultData, requiredLength, (delay_InUS / portTICK_PERIOD_MS));

    if (resultLength != requiredLength) {
        // TODO Uart read data exception
//        throw UartReadException();
    }

    return resultData;
}

Bytes UartNetworkConnector::readBytes(size_t requiredLength)
{
    std::lock_guard<std::mutex> lockGuard(m_mutex);

    Bytes resultData;

    auto delay_InUS = calculateDelayForWaitDataFromUartInMilliseconds(requiredLength, m_baudRate);
    auto resultLength = uart_read_bytes(m_numberOfUartPorts, &resultData, requiredLength, (delay_InUS / portTICK_PERIOD_MS));

    if (resultLength != requiredLength) {
        // TODO Uart read data exception
    }

    return resultData;
}

void UartNetworkConnector::write(Bytes const& data)
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
        throw ECU::NetworkException(errorMessage, errorCode);
    }

    writeDataToUart(data, m_numberOfUartPorts);
}

size_t UartNetworkConnector::getBufferSize() const
{
    size_t bufferSize = 0;

    uart_get_buffered_data_len(m_numberOfUartPorts, &bufferSize);

    return bufferSize;
}

} // namespace ECU