cmake_minimum_required(VERSION 3.17)

set(SOURCES
        main.cpp
        Controller.cpp
        SetupButton.cpp
        node/EcuNode.cpp
        node/MotorNode.cpp
        node/ControllerNode.cpp
        node/AcceleratorNode.cpp
        node/SetupButtonNode.cpp)

idf_component_register(SRCS ${SOURCES} INCLUDE_DIRS .)


