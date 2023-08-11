cmake_minimum_required(VERSION 3.26.4)

set(SOURCES
        main.cpp)

idf_component_register(SRCS ${SOURCES} INCLUDE_DIRS .)


