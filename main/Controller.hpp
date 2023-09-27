// Copyright 2023 Pavel Suprunov
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

//
// Created by jadjer on 29.08.22.
//

#pragma once

#include <cstdlib>
#include <functional>

using ControllerChangeValueCallbackFunction = std::function<void(uint32_t)>;

class Controller {
public:
    Controller();

public:
    void registerChangeValueCallback(ControllerChangeValueCallbackFunction const &changeValueCallbackFunction);

public:
    void setRPM(uint32_t revolutionPerMinute);

    void setSpeed(uint32_t speedInKilometersPerHour);

    void setAcceleration(uint32_t accelerationValue);

    void setClutch(bool clutchIsEnabled);

public:
    void enable();

    void disable();

private:
    void process();

private:
    ControllerChangeValueCallbackFunction m_changeValueCallbackFunction = nullptr;

private:
    bool m_clutchIsEnabled;
    uint32_t m_accelerationValue;
    uint32_t m_revolutionPerMinute;
    uint32_t m_speed_InKilometersPerHour;
    uint32_t m_cruiseSpeed_InKilometersPerHour;
};
