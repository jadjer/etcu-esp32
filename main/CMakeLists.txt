cmake_minimum_required(VERSION 3.28.1)

set(INCLUDE_DIR
        include
)

set(SOURCES
        app_main.cpp
)



idf_component_register(SRCS ${SOURCES} INCLUDE_DIRS ${INCLUDE_DIR})
