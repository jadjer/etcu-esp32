cmake_minimum_required(VERSION 3.28.1)

set(INCLUDE_DIRS
        include
)

set(SOURCES
        app_main.cpp

        src/Updater.cpp
        src/Bluetooth.cpp
        src/Configuration.cpp
)

idf_component_register(SRCS ${SOURCES} INCLUDE_DIRS ${INCLUDE_DIRS})
