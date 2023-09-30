cmake_minimum_required(VERSION 3.17)

set(SOURCES
        src/Motor.cpp)

set(COMPONENTS
        gpio
        executor)

idf_component_register(SRCS ${SOURCES} INCLUDE_DIRS include REQUIRES ${COMPONENTS})