cmake_minimum_required(VERSION 3.17)

set(SOURCES
        main.cpp

#        Limiter.cpp
#        ModeButton.cpp
#        Accelerator.cpp
#        SetupButton.cpp
#        EtcController.cpp
#
#        stepper/MotorDriver.cpp
#        stepper/MotorController.cpp

        bluetooth/ThrottlePositionCharacteristic.cpp
)

idf_component_register(SRCS ${SOURCES} INCLUDE_DIRS .)
