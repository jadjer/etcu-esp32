#pragma once


#include "ecu/UartNetworkConnector.h"
#include "ecu/INetwork.h"

#include <cstdint>


namespace NetworkConnector
{

class UartNetworkConnector : public INetworkConnector
{
public:
    UartNetworkConnector(uint8_t numberOfRxPin, uint8_t numberOfTxPin, uint8_t numberOfUartPorts);

public:
    void Write(std::vector<uint8_t> const& data) override;

    CommandResultPtr Read() override;

private:
    void Connect();

private:
    std::mutex m_mutex;

    uint8_t const m_numberOfRxPin;
    uint8_t const m_numberOfTxPin;
    uint8_t const m_numberOfUartPorts;
};

} // namespace NetworkConnector