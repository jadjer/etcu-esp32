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

#include "TwistPosition.hpp"

#include <esp_log.h>
#include <utility>

auto const TAG = "Twist Position";

// Функция для перевода значения из одного диапазона в другой с возвратом std::uint8_t
std::uint8_t mapValue(float value, float fromMin, float fromMax, float toMin = 0, float toMax = 100) {
  // Проверка на случай, если исходный диапазон нулевой
  if (fromMin == fromMax) {
    fromMin -= 1;
    fromMax += 1;
  }

  // Вычисление пропорции
  float const fromRange = fromMax - fromMin;
  float const toRange = toMax - toMin;

  // Перевод значения в новый диапазон
  float const scaledValue = (value - fromMin) / fromRange;
  float result = toMin + (scaledValue * toRange);

  if (result < toMin) {
    result = toMin;
  }
  if (result > toMax) {
    result = toMax;
  }

  result += 0.5f;

  return static_cast<std::uint8_t>(result);
}

TwistPosition::TwistPosition() : m_adc(0),
                                 m_sensor1(m_adc.createChannel(3)),
                                 m_sensor2(m_adc.createChannel(6)),
                                 m_positionMinSensor1(1000),
                                 m_positionMaxSensor1(3000),
                                 m_positionMinSensor2(500),
                                 m_positionMaxSensor2(1500) {

//  m_sensor1->enableFilter();
//  m_sensor2->enableFilter();
}

void TwistPosition::registerErrorCallback(TwistPosition::ErrorCallback callback) {
  m_errorCallback = std::move(callback);
}

void TwistPosition::registerPositionChangeCallback(TwistPosition::PositionChangeCallback callback) {
  m_positionChangeCallback = std::move(callback);
}

TwistPosition::Position TwistPosition::getPosition() const {
  return m_position;
}

void TwistPosition::process() {
  auto const voltageSensor1 = m_sensor1->getVoltage();
  auto const positionSensor1 = mapValue(voltageSensor1, m_positionMinSensor1, m_positionMaxSensor1, 0, 100);

  auto const voltageSensor2 = m_sensor2->getVoltage();
  auto const positionSensor2 = mapValue(voltageSensor2, m_positionMinSensor2, m_positionMaxSensor2, 0, 100);

  auto diffSensorValue = positionSensor1 - positionSensor2;
  if (diffSensorValue < 0) {
    diffSensorValue *= -1;
  }

  if (diffSensorValue > 5) {
    m_position = 0;

    if (m_errorCallback) {
      m_errorCallback();
    }
  } else {
    m_position = positionSensor1;
  }

//  ESP_LOGI(TAG, "Sensor 1: voltage %d, position %d; Sensor 2: voltage %d, position %d", voltageSensor1, positionSensor1, voltageSensor2, positionSensor2);

  if (m_positionChangeCallback) {
    m_positionChangeCallback(m_position);
  }
}
