cmake_minimum_required(VERSION 3.17)

set(SOURCES
        main.cpp
        Scaller.cpp
        Limiter.cpp
        MotorDriver.cpp
        Formatter.cpp
        Accelerator.cpp
        Accelerator.cpp
        ModeButton.cpp
        SetupButton.cpp
        Controller.cpp
        MotorController.cpp
)

idf_component_register(SRCS ${SOURCES} INCLUDE_DIRS .)


