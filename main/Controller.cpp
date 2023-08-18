//
// Created by jadjer on 29.08.22.
//

#include "Controller.hpp"

Controller::Controller() :
        _clutchIsEnabled(true),
        _accelerationValue(0),
        _revolutionPerMinute(0),
        _minimalAccelerationValue(0),
        _speed_InKilometersPerHour(0),
        _cruiseSpeed_InKilometersPerHour(0){

}

Controller::~Controller() = default;

void Controller::registerChangeValueCallback(ControllerChangeValueCallbackFunction const &controllerChangeValueCallbackFunction) {
    _controllerChangeValueCallbackFunction = controllerChangeValueCallbackFunction;
}

void Controller::setRPM(uint32_t revolutionPerMinute) {
    _revolutionPerMinute = revolutionPerMinute;
}

void Controller::setSpeed(uint32_t speedInKilometersPerHour) {
    _speed_InKilometersPerHour = speedInKilometersPerHour;
}

void Controller::setClutch(bool clutchIsEnabled) {
    _clutchIsEnabled = clutchIsEnabled;
}

void Controller::setAcceleration(uint32_t accelerationValue) {
    _accelerationValue = accelerationValue;
}

void Controller::enable() {
    _minimalAccelerationValue = _accelerationValue;
    _cruiseSpeed_InKilometersPerHour = _speed_InKilometersPerHour;
}

void Controller::disable() {
    _minimalAccelerationValue = 0;
    _cruiseSpeed_InKilometersPerHour = 0;
}

void Controller::process() {
    auto value = _accelerationValue;
    if (value < _minimalAccelerationValue) {
        value = _minimalAccelerationValue;
    }

    if (_cruiseSpeed_InKilometersPerHour > 0) {
        if (_speed_InKilometersPerHour < _cruiseSpeed_InKilometersPerHour) {
            value += 10;
        }
        if (_speed_InKilometersPerHour > _cruiseSpeed_InKilometersPerHour) {
            value -= 10;
        }

        if (_revolutionPerMinute < 2500 or _revolutionPerMinute > 6000 or not _clutchIsEnabled) {
            _cruiseSpeed_InKilometersPerHour = 0;
        }
    }

    if (_revolutionPerMinute > 6000) {
        _minimalAccelerationValue = 0;
    }

    if (_controllerChangeValueCallbackFunction) {
        _controllerChangeValueCallbackFunction(value);
    }
}
