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

#pragma once

#include "Common/Exception.hpp"

/**
 * @namespace ecu
 */
namespace ecu
{

/**
 * @class NetworkException
 */
class NetworkException : public Common::Exception
{
public:
    NetworkException(std::string const& message, ErrorCode::Enum const errorCode) : Common::Exception(message, errorCode) {}
};

/**
 * @class UartWriteError
 */
class UartWriteError : public Common::Exception
{
public:
    UartWriteError(std::string const& message, ErrorCode::Enum errorCode) : Common::Exception(message, errorCode) {}
};

class KLineConnectException : public Common::Exception
{
public:
    KLineConnectException(std::string const& message, ErrorCode::Enum errorCode) : Common::Exception(message, errorCode) {}
};

class UartReadException : public Common::Exception
{
public:
    UartReadException(std::string const& message, ErrorCode::Enum errorCode) : Common::Exception(message, errorCode) {}
};

} // namespace ECU
