// Copyright 2025 Pavel Suprunov
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

#include "motor/Motor.hpp"

#include <foc/motor/BLDCMotor.hpp>
#include <esp_log.h>

auto const TAG = "Motor";

Motor::Motor() : m_encoder(std::make_unique<AS5600>()),
                 m_motor(std::make_unique<foc::BLDCMotor>(22, 0.2, 360, 20)) {

  m_motor->linkEncoder(std::move(m_encoder));
//  m_motor->linkCurrentSense()

  m_motor->init();
  m_motor->initFOC();
}

void Motor::setPosition(Motor::Position position) {
  m_motor->move(position);
}

void Motor::process() {
  m_motor->loopFOC();
}
