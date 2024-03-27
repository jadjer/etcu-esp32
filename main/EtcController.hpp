// Copyright 2024 Pavel Suprunov
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
// Created by jadjer on 29.08.22.
//

#pragma once

#include <cstdlib>
#include <functional>

using EtcControllerChangeValueCallbackFunction = std::function<void(uint32_t)>;

class EtcController {
public:
  EtcController();

  ~EtcController();

public:
  void registerChangeValueCallback(EtcControllerChangeValueCallbackFunction const &controllerChangeValueCallbackFunction);

public:
  void setRPM(uint32_t revolutionPerMinute);

  void setSpeed(uint32_t speedInKilometersPerHour);

  void setClutch(bool clutchIsEnabled);

  void setAcceleration(uint32_t accelerationValue);

public:
  void modeEnable();

  void modeDisable();

public:
  void process();

private:
  EtcControllerChangeValueCallbackFunction m_changeValueCallbackFunction;

private:
  bool m_clutchIsEnabled;

private:
  uint32_t m_accelerationValue;
  uint32_t m_revolutionPerMinute;
  uint32_t m_minimalAccelerationValue;
  uint32_t m_speed_InKilometersPerHour;
  uint32_t m_cruiseSpeed_InKilometersPerHour;
};
