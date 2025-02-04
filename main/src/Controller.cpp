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

#include "Controller.hpp"

#include <esp_log.h>
#include <esp_timer.h>
#include <utility>

auto const TAG = "Controller";
auto const MICROSECONDS_IN_SECOND = 1000000;
auto const DEAD_TIME = 5 * MICROSECONDS_IN_SECOND;
auto const SPEED_MODE_CRUISE = 60;
auto const POSITION_MINIMAL = 0;
auto const POSITION_MAXIMAL = 100;

// P (kp): Отвечает за быстрое увеличение дроссельной заслонки при отклонении скорости от заданной.
// I (ki): Устраняет ситуацию, когда автомобиль не может достичь заданной скорости из-за нагрузки (например, подъём в гору).
// D (kd): Предотвращает резкие изменения положения дроссельной заслонки, когда скорость приближается к заданной.

/*
 * Настройка коэффициентов:

    Начните с P:
        Установите ki = 0 и kd = 0.
        Увеличивайте kp, пока система не начнёт быстро реагировать на ошибку, но без сильных колебаний.

    Добавьте I:
        Увеличивайте ki, чтобы устранить статическую ошибку.
        Следите, чтобы система не стала слишком медленной или не начала колебаться.

    Добавьте D:
        Увеличивайте kd, чтобы уменьшить перерегулирование и колебания.
        Убедитесь, что система не становится слишком "нервной" (резкие изменения управления).
 */

Controller::Controller(ConfigurationPtr configuration) : m_configuration(std::move(configuration)),
                                                         m_pid(5, 0, 0),
                                                         m_throttlePositionMaximal(POSITION_MAXIMAL) {
  m_pid.setPoint(POSITION_MINIMAL);
}

void Controller::registerErrorCallback(Controller::ErrorCallback callback) {
  m_errorCallback = std::move(callback);
}

void Controller::registerCruiseEnableCallback(Controller::CruiseEnabledCallback callback) {
  m_cruiseEnabledCallback = std::move(callback);
}

void Controller::registerPositionUpdateCallback(Controller::PositionUpdateCallback callback) {
  m_positionUpdateCallback = std::move(callback);
}

void Controller::setRPM(Controller::RPM const rpm) {
  m_rpm = rpm;
  m_rpmLastUpdate = esp_timer_get_time();
}

void Controller::setSpeed(Controller::Speed const speed) {
  m_speed = speed;
  m_speedLastUpdate = esp_timer_get_time();
}

void Controller::setTemperature(Controller::Temp const temp) {
  m_temp = temp;
}

void Controller::setTwistPosition(Controller::Position const position) {
  m_twistPosition = position;
}

void Controller::setThrottlePosition(Controller::Position const position) {
  m_throttlePosition = position;
}

void Controller::enableGuardMode() {
  m_throttlePositionMaximal = POSITION_MINIMAL;
}

void Controller::holdRPM() {
  if (m_speed >= SPEED_MODE_CRUISE) {
    return;
  }

  m_throttlePositionMinimal = m_throttlePosition;
}

void Controller::releaseRPM() {
  if (m_speed > SPEED_MODE_CRUISE) {
    return;
  }

  m_throttlePositionMinimal = POSITION_MINIMAL;
}

void Controller::enableCruiseMode() {
  if (m_speed < SPEED_MODE_CRUISE) {
    return;
  }

  m_pid.setPoint(m_speed);

  if (m_cruiseEnabledCallback) {
    m_cruiseEnabledCallback(true);
  }
}

void Controller::disableCruiseMode() {
  m_pid.setPoint(POSITION_MINIMAL);

  if (m_cruiseEnabledCallback) {
    m_cruiseEnabledCallback(false);
  }
}

void Controller::process() {
  auto throttlePosition = m_pid.compute(m_speed);

  if (throttlePosition < m_twistPosition) {
    throttlePosition = m_twistPosition;
  }

  if (throttlePosition < m_throttlePositionMinimal) {
    throttlePosition = m_throttlePositionMinimal;
  }

  if (throttlePosition > m_throttlePositionMaximal) {
    throttlePosition = m_throttlePositionMaximal;
  }

  if (m_positionUpdateCallback) {
    m_positionUpdateCallback(throttlePosition);
  }
}
