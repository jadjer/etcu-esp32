#pragma once

#include "Expected.h"

#include "esp_err.h"

#include <cstdint>
#include <memory>
#include <vector>


namespace
{

struct CommandResult
{
    uint8_t Code;
    uint8_t Command;
    uint8_t Length;
    uint8_t Checksum;
    std::vector<uint8_t> data;
};

using CommandResultPtr = std::unique_ptr<CommandResult>;

} // namespace

namespace NetworkConnector
{

class INetworkConnector
{
public:
    virtual ~INetworkConnector() = default;

public:
    virtual CommandResultPtr Read() = 0;
    virtual void Write(std::vector<uint8_t> const& data) = 0;
};

} // namespace NetworkConnector
