#include <utility>

#include "Common/Exception.hpp"

namespace Common
{

Exception::Exception(std::string message, ErrorCode::Enum const code) : m_errorMessage(std::move(message)), m_errorCode(code) {}

char const* Exception::what() const noexcept
{
    return m_errorMessage.c_str();
}

ErrorCode::Enum Exception::code() const noexcept
{
    return m_errorCode;
}

std::string const& Exception::message() const noexcept
{
    return m_errorMessage;
}

} // namespace Common
