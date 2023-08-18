cmake_minimum_required(VERSION 3.17)

set(SOURCES
        src/utils.cpp
        src/UartProtocol.cpp
        src/ElectronicControlUnit.cpp)

set(COMPONENTS
        driver)

idf_component_register(SRCS ${SOURCES} INCLUDE_DIRS include REQUIRES ${COMPONENTS})
