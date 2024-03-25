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
// Created by jadjer on 3/24/24.
//

#pragma once

#include "executor/Node.hpp"
#include "motor/MotorController.hpp"
#include "motor/interface/ILimiter.hpp"
#include "motor/driver/interface/IDriver.hpp"

class MotorController : public executor::Node {
public:
  MotorController();
  ~MotorController() override = default;

public:
  void setSpeed(float speed);
  void setAcceleration(float acceleration);
  void setDeceleration(float deceleration);

public:
  void setPosition(int32_t position);

private:
  void process() override;

private:
  MotorControllerPtr m_motorController;
};
