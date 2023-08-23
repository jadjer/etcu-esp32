#pragma once

#include "Exception.h"


namespace Common
{

class NetworkException : public Exception
{
public:
    NetworkException(std::string const& message, ErrorCode::Enum const errorCode) : Exception(message, errorCode) {}
};

class UartWriteError : public Exception
{
public:
    UartWriteError(std::string const& message, ErrorCode::Enum errorCode) : Exception(message, errorCode) {}
};

} // namespace Common
