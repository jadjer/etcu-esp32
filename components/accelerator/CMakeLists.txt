cmake_minimum_required(VERSION 3.17)

set(SOURCES
        src/Scaler.cpp
        src/Formatter.cpp
        src/Accelerator.cpp)

set(COMPONENTS
        esp_adc
        executor)

idf_component_register(SRCS ${SOURCES} INCLUDE_DIRS include REQUIRES ${COMPONENTS})
