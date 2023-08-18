cmake_minimum_required(VERSION 3.17)

set(SOURCES
        src/MotorDriver.cpp)

set(COMPONENTS
        driver
        esp_timer)

idf_component_register(SRCS ${SOURCES} INCLUDE_DIRS include REQUIRES ${COMPONENTS})