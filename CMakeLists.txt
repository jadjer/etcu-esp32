cmake_minimum_required(VERSION 3.17)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

project(electronic_throttle
        VERSION 0.6.2
        DESCRIPTION "Honda motorcycle electronic throttle based on esp32"
        HOMEPAGE_URL https://github.com/jadjer/electronic-throttle-control-unit-esp32
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
