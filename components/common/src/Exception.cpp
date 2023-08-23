#include "Exception.h"

namespace Common
{

Exception::Exception(std::string const& message, ErrorCode::Enum const code) : m_errorMessage(message), m_errorCode(code) {}

const char* Exception::what() const noexcept
{
    return m_errorMessage.c_str();
}

ErrorCode::Enum Exception::Code() const noexcept
{
    return m_errorCode;
}

std::string const& Exception::Message() const noexcept
{
    return m_errorMessage;
}

} // namespace Common