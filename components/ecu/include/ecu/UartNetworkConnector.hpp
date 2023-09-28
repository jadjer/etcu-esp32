#pragma once

#include <mutex>

#include "ecu/interface/UartNetworkConnector.hpp"

/**
 * @namespace ecu
 */
namespace ecu
{

/**
 * @class UartNetworkConnector
 */
class UartNetworkConnector : public ecu::interface::UartNetworkConnector
{
public:
    UartNetworkConnector(Byte numberOfRxPin, Byte numberOfTxPin, Byte numberOfUartPorts);

public:
    void connect() override;

public:
    [[nodiscard]] size_t getBufferSize() const;

public:
    Byte readByte() override;
    Bytes readBytes(size_t requiredLength) override;
    void write(Bytes const& data) override;

private:
    std::mutex m_mutex;

protected:
    uint8_t const m_numberOfRxPin;
    uint8_t const m_numberOfTxPin;
    uint8_t const m_numberOfUartPorts;
    uint16_t const m_baudRate;
};

} // namespace ecu