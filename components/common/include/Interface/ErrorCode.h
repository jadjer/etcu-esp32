#pragma once

#include "esp_err.h"


struct ErrorCode
{
    enum Enum
    {
        Success = 0,

        OutOfMemory,
        InvalidArgument,
        InvalidState,
        ResourceNotFound,
        OperationNotSupported,
        TimeOutOperation,
        InvalidResponse,
        InvalidChecksum,
        InvalidVersion,
        InvalidMacAddress,
        OperationAborted,

        Count = 13
    };

    static Enum EspStatusCodeToErrorCode(esp_err_t  const espError);

    static esp_err_t ErrorCodeToEspStatus(Enum const errorCode);

};

