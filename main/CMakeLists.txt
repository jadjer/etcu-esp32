cmake_minimum_required(VERSION 3.28.1)

set(INCLUDE_DIRS
        include
)

set(SOURCES
        app_main.cpp

        src/Switch.cpp
        src/Indicator.cpp
        src/Configuration.cpp

        src/sensor/Throttle.cpp
        src/sensor/SensorBase.cpp
        src/sensor/TwistPosition.cpp
        src/sensor/TwistPositionSensor.cpp

        src/bluetooth/Bluetooth.cpp
        src/bluetooth/ConfigurationCharacteristicCallback.cpp

        src/bluetooth/ota/Updater.cpp
        src/bluetooth/ota/MessageHandler.cpp
        src/bluetooth/ota/UpdateCharacteristicCallback.cpp

        src/controller/Controller.cpp
        src/controller/ControllerBase.cpp
)

idf_component_register(SRCS ${SOURCES} INCLUDE_DIRS ${INCLUDE_DIRS})
