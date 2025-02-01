cmake_minimum_required(VERSION 3.28.1)

set(INCLUDE_DIRS
        include
)

set(SOURCES
        app_main.cpp

        src/App.cpp
        src/HECU.cpp
        src/Indicator.cpp
        src/Controller.cpp
        src/ModeButton.cpp


        src/sensor/Throttle.cpp
        src/sensor/SensorBase.cpp
        src/sensor/TwistPosition.cpp
        src/sensor/TwistPositionSensor.cpp

        src/switch/BreakSwitch.cpp
        src/switch/GuardSwitch.cpp
        src/switch/ClutchSwitch.cpp

        src/bluetooth/Bluetooth.cpp
        src/bluetooth/ConfigurationCharacteristicCallback.cpp

        src/bluetooth/ota/Updater.cpp
        src/bluetooth/ota/MessageHandler.cpp
        src/bluetooth/ota/UpdateCharacteristicCallback.cpp

        src/configuration/Configuration.cpp
)

idf_component_register(SRCS ${SOURCES} INCLUDE_DIRS ${INCLUDE_DIRS})
