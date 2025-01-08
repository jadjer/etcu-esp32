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
// Created by jadjer on 3/18/24.
//

#include "controller/Controller.hpp"

#include <esp_log.h>
#include <esp_sleep.h>

auto constexpr TAG = "Controller";

auto constexpr MICROSECONDS_PER_SECOND = 1000000;

Controller::Controller(ConfigurationPtr configuration) : m_configuration(std::move(configuration)),
                                                         m_pumpPtr(std::make_unique<Pump>(m_configuration->getPumpPin())),
                                                         m_timerPtr(std::make_unique<Timer>()),
                                                         m_wheelSensorPtr(std::make_unique<WheelSensor>(m_configuration->getWheelSensorPin(), m_configuration->getWheelLength())),
                                                         m_externalPowerPtr(std::make_unique<ExternalPower>(m_configuration->getExternalPowerPin())) {

  auto const nextDistance = m_configuration->getNextDistance();
  auto const totalDistance = m_configuration->getTotalDistance();

  if (nextDistance <= totalDistance) {
    auto const distanceForEnable = m_configuration->getDistanceForEnable();

    m_configuration->saveNextDistance(totalDistance + distanceForEnable);
  }
}

void Controller::spinOnce() {
  if (not m_externalPowerPtr->isEnabled()) {
    sleep();
  }

  auto const actualDistance = m_wheelSensorPtr->getDistance();
  auto const savedDistance = m_configuration->getTotalDistance();
  auto const totalDistance = actualDistance + savedDistance;
  auto const nextDistance = m_configuration->getNextDistance();

  if (totalDistance >= nextDistance) {
    ESP_LOGI(TAG, "The required distance has been covered for lubrication");
    m_configuration->setLubricate(true);
  }

  auto const isManualLubricate = m_configuration->isManualLubricate();
  if (isManualLubricate) {
    ESP_LOGI(TAG, "Forced lubrication mode");
    m_configuration->setLubricate(true);
  }

  auto const isLubricate = m_configuration->isLubricate();
  if (not isLubricate) {
    ESP_LOGI(TAG, "Total distance: %f[m], next distance: %f[m]", totalDistance, nextDistance);
    return;
  }

  auto const speed = m_wheelSensorPtr->getSpeed();
  auto const minimalSpeed = m_configuration->getMinimalSpeed();

  if (speed >= minimalSpeed) {
    pumpEnable();
    return;
  }

  ESP_LOGI(TAG, "The vehicle's speed is too low (speed: %f[m/s], minimal speed: %f[m/s])", speed, minimalSpeed);
  pumpDisable();
}

void Controller::sleep() {
  ESP_LOGI(TAG, "The external power is turned off. Falling asleep");

  auto const actualDistance = m_wheelSensorPtr->getDistance();
  auto const savedDistance = m_configuration->getTotalDistance();
  auto const totalDistance = actualDistance + savedDistance;

  m_configuration->saveTotalDistance(totalDistance);

  pumpDisable();
  esp_deep_sleep_start();
}

void Controller::pumpEnable() {
  if (m_pumpPtr->isEnabled()) {
    return;
  }

  m_pumpPtr->enable();

  if (not m_pumpPtr->isEnabled()) {
    ESP_LOGE(TAG, "The pump is not enabled");
    return;
  }

  auto const pumpTimeout_InSeconds = m_configuration->getPumpTimeout();

  ESP_LOGI(TAG, "The pump is turned on for %lu seconds", pumpTimeout_InSeconds);

  auto const pumpTimeout_InMicroseconds = pumpTimeout_InSeconds * MICROSECONDS_PER_SECOND;

  m_timerPtr->start(pumpTimeout_InMicroseconds, [this] {
    auto const actualDistance = m_wheelSensorPtr->getDistance();
    auto const savedDistance = m_configuration->getTotalDistance();
    auto const totalDistance = actualDistance + savedDistance;
    auto const distanceForEnable = m_configuration->getDistanceForEnable();
    auto const nextDistance = totalDistance + distanceForEnable;

    m_configuration->setLubricate(false);
    m_configuration->setManualLubricate(false);
    m_configuration->saveTotalDistance(totalDistance);
    m_configuration->saveNextDistance(nextDistance);

    ESP_LOGI(TAG, "End of the lubrication cycle");

    pumpDisable();
  });
}

void Controller::pumpDisable() {
  if (not m_pumpPtr->isEnabled()) {
    return;
  }

  m_pumpPtr->disable();

  if (m_pumpPtr->isEnabled()) {
    ESP_LOGE(TAG, "The pump is not disabled");
    return;
  }

  ESP_LOGI(TAG, "The pump is disabled");

  m_timerPtr->stop();
}
