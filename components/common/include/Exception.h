#include "Interface/IException.h"

#include <string>


namespace Common
{

class Exception : public IException
{
public:
    Exception(std::string const& message, ErrorCode::Enum const code);

    [[nodiscard]] char const* what() const noexcept override;

    [[nodiscard]] std::string const& Message() const noexcept override;

    [[nodiscard]] ErrorCode::Enum Code() const noexcept override;

private:
    std::string const m_errorMessage;
    ErrorCode::Enum const m_errorCode;
};

} // namespace Common
