cmake_minimum_required(VERSION 3.17)

set(SOURCES
        src/utils.cpp
        src/UartNetworkConnector.cpp
        src/UartProtocol.cpp
        src/BaseElectronicControlUnit.cpp)

set(COMPONENTS
        common
        driver)

idf_component_register(SRCS ${SOURCES} INCLUDE_DIRS include REQUIRES ${COMPONENTS})
