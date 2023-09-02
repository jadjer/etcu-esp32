#include "Common/Interface/ErrorCode.hpp"

esp_err_t ErrorCode::ErrorCodeToEspStatus(ErrorCode::Enum errorCode)
{
    switch (errorCode)
    {
    case Success:
        return ESP_OK;
    case OutOfMemory:
        return ESP_ERR_NO_MEM;
    case InvalidArgument:
        return ESP_ERR_INVALID_ARG;
    case InvalidState:
        return ESP_ERR_INVALID_STATE;
    case ResourceNotFound:
        return ESP_ERR_NOT_FOUND;
    case OperationNotSupported:
        return ESP_ERR_NOT_SUPPORTED;
    case TimeOutOperation:
        return ESP_ERR_TIMEOUT;
    case InvalidResponse:
        return ESP_ERR_INVALID_RESPONSE;
    case InvalidChecksum:
        return ESP_ERR_INVALID_CRC;
    case InvalidVersion:
        return ESP_ERR_INVALID_VERSION;
    case InvalidMacAddress:
        return ESP_ERR_INVALID_MAC;
    case OperationAborted:
        return ESP_ERR_NOT_FINISHED;
    default:
        std::abort(); // developer attention - we are not expect use count or some another as parameter of type
    }
}

ErrorCode::Enum ErrorCode::EspStatusCodeToErrorCode(esp_err_t const espError)
{
    switch (espError)
    {
    case ESP_OK:
        return ErrorCode::Success;
    case ESP_ERR_NO_MEM:
        return ErrorCode::OutOfMemory;
    case ESP_ERR_INVALID_ARG:
        return ErrorCode::InvalidArgument;
    case ESP_ERR_INVALID_STATE:
        return ErrorCode::InvalidState;
    case ESP_ERR_NOT_FOUND:
        return ErrorCode::ResourceNotFound;
    case ESP_ERR_NOT_SUPPORTED:
        return ErrorCode::OperationNotSupported;
    case ESP_ERR_TIMEOUT:
        return ErrorCode::TimeOutOperation;
    case ESP_ERR_INVALID_RESPONSE:
        return ErrorCode::InvalidResponse;
    case ESP_ERR_INVALID_CRC:
        return ErrorCode::InvalidChecksum;
    case ESP_ERR_INVALID_VERSION:
        return ErrorCode::InvalidVersion;
    case ESP_ERR_INVALID_MAC:
        return ErrorCode::InvalidMacAddress;
    case ESP_ERR_NOT_FINISHED:
        return ErrorCode::OperationAborted;
    default:
        // Handle other cases if necessary
        std::abort();
    }
}
