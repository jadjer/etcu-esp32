cmake_minimum_required(VERSION 3.17)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

project(TCU
        VERSION 0.6.2
        DESCRIPTION "Electronic throttle control unit for Honda Varadero based on esp32-s3"
        HOMEPAGE_URL https://github.com/jadjer/electronic-throttle-control-unit-esp32
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED True)
