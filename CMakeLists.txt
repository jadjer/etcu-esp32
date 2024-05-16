cmake_minimum_required(VERSION 3.28.1)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

project(ETCU
        VERSION 0.6.2
        DESCRIPTION "Electronic throttle control unit for Honda Varadero based on esp32-s3"
        HOMEPAGE_URL https://github.com/jadjer/electronic-throttle-control-unit-esp32
        LANGUAGES CXX)

