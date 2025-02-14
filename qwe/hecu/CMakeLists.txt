cmake_minimum_required(VERSION 3.17)

set(CMAKE_CXX_STANDARD 17)

set(SOURCES
        ECU.cpp
        utils.cpp
        Protocol.cpp)

set(COMPONENTS
        )

idf_component_register(SRCS ${SOURCES} INCLUDE_DIRS include REQUIRES ${COMPONENTS})
