#pragma once

#include "ErrorCode.h"

#include <system_error>
#include <string>


class IException : public std::system_error
{
public:
    virtual char const* what() const noexcept  = 0;

    virtual std::string const& Message() const noexcept = 0;

    virtual ErrorCode::Enum Code() const noexcept = 0;
};
