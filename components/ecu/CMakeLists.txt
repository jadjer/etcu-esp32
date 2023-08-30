cmake_minimum_required(VERSION 3.17)

set(SOURCES
        src/UartNetworkConnector.cpp
        src/HondaECU.cpp
        src/KLineNetworkConnector.cpp)

set(COMPONENTS
        common
        driver
        executor)

idf_component_register(SRCS ${SOURCES} INCLUDE_DIRS include REQUIRES ${COMPONENTS})
