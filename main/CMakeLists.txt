cmake_minimum_required(VERSION 3.17)

set(SOURCES
        main.cpp
        Controller.cpp
        SetupButton.cpp
        ModeButton.cpp)

idf_component_register(SRCS ${SOURCES} INCLUDE_DIRS .)


