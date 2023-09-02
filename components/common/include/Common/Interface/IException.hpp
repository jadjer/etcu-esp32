#pragma once

#include "Common/Interface/ErrorCode.hpp"

#include <system_error>
#include <string>


class IException : public std::system_error
{
public:
    virtual char const* what() const noexcept = 0;

    virtual std::string const& message() const noexcept = 0;

    virtual ErrorCode::Enum code() const noexcept = 0;
};
