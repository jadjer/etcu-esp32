cmake_minimum_required(VERSION 3.17)

set(SOURCES
        main.cpp
        Limiter.cpp
        ModeButton.cpp
        Accelerator.cpp
        MotorDriver.cpp
        SetupButton.cpp
        EtcController.cpp
        MotorController.cpp
)

idf_component_register(SRCS ${SOURCES} INCLUDE_DIRS .)


