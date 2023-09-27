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

#include "Interface/IException.hpp"

#include <string>


namespace Common
{

class Exception : public IException
{
public:
    Exception(std::string  message, ErrorCode::Enum code);

    [[nodiscard]] char const* what() const noexcept override;

    [[nodiscard]] std::string const& message() const noexcept override;

    [[nodiscard]] ErrorCode::Enum code() const noexcept override;

private:
    std::string const m_errorMessage;
    ErrorCode::Enum const m_errorCode;
};

} // namespace Common
