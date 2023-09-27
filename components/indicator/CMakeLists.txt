cmake_minimum_required(VERSION 3.17)

set(SOURCES
        src/Indicator.cpp
        src/BlinkIndicator.cpp
        src/ErrorCodeIndicator.cpp)

set(COMPONENTS
        driver
        executor)

idf_component_register(SRCS ${SOURCES} INCLUDE_DIRS include REQUIRES ${COMPONENTS})
