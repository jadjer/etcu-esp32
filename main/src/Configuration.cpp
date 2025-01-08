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
// Created by jadjer on 9/20/24.
//

#include "configuration/Configuration.hpp"

#include <cmath>
#include <cstring>
#include <nvs.h>
#include <nvs_flash.h>

#include "sdkconfig.h"

esp_err_t nvs_set_float(nvs_handle_t handle, char const *key, float value) {
  std::uint32_t buf = 0;

  memcpy(&buf, &value, sizeof(float));

  return nvs_set_u32(handle, key, buf);
}

esp_err_t nvs_get_float(nvs_handle_t handle, char const *key, float *value) {
  std::uint32_t buf = 0;

  esp_err_t err = nvs_get_u32(handle, key, &buf);
  if (err == ESP_OK) {
    memcpy(value, &buf, sizeof(float));
  }

  return err;
}

auto constexpr METERS_IN_INCH = 0.0254;

Configuration::Configuration() : m_isManualLubricate(false),
                                 m_storageHandle(0) {

  esp_err_t err = nvs_flash_init();
  if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    err = nvs_flash_init();
  }
  ESP_ERROR_CHECK(err);

  ESP_ERROR_CHECK(nvs_open("storage", NVS_READWRITE, &m_storageHandle));
}

Configuration::~Configuration() {
  ESP_ERROR_CHECK(nvs_commit(m_storageHandle));
  nvs_close(m_storageHandle);
}

bool Configuration::isLubricate() const {
  std::uint8_t lubricate = 0;

  nvs_get_u8(m_storageHandle, "lubricate", &lubricate);

  return static_cast<bool>(lubricate);
}

bool Configuration::isManualLubricate() const {
  return m_isManualLubricate;
}

std::uint8_t Configuration::getExternalPowerPin() const {
  std::uint8_t const externalPowerPin = CONFIG_EXTERNAL_POWER_PIN;

  return externalPowerPin;
}

std::uint8_t Configuration::getPumpPin() const {
  std::uint8_t const pumpPin = CONFIG_PUMP_PIN;

  return pumpPin;
}

std::uint8_t Configuration::getWheelSensorPin() const {
  std::uint8_t const wheelSensorPin = CONFIG_WHEEL_SENSOR_PIN;

  return wheelSensorPin;
}

std::uint32_t Configuration::getPumpTimeout() const {
  std::uint32_t pumpTimeout = CONFIG_PUMP_TIMEOUT;

  nvs_get_u32(m_storageHandle, "pump_timeout", &pumpTimeout);

  return pumpTimeout;
}

float Configuration::getWheelLength() const {
  std::uint32_t const wheelDiameter_InInches = CONFIG_WHEEL_DIAMETER;

  float const wheelDiameter_InMeter = static_cast<float>(wheelDiameter_InInches) * METERS_IN_INCH;
  float wheelLength = wheelDiameter_InMeter * static_cast<float>(M_PI);

  nvs_get_float(m_storageHandle, "wheel_length", &wheelLength);

  return wheelLength;
}

float Configuration::getMinimalSpeed() const {
  float minimalSpeed = CONFIG_WHEEL_MINIMAL_SPEED;

  nvs_get_float(m_storageHandle, "minimal_speed", &minimalSpeed);

  return minimalSpeed;
}

float Configuration::getDistanceForEnable() const {
  float distanceForEnable = CONFIG_DISTANCE_OF_ENABLE;

  nvs_get_float(m_storageHandle, "distance_for_enable", &distanceForEnable);

  return distanceForEnable;
}

float Configuration::getTotalDistance() const {
  float totalDistance = 0;

  nvs_get_float(m_storageHandle, "total_distance", &totalDistance);

  return totalDistance;
}

float Configuration::getNextDistance() const {
  float nextDistance = 0;

  nvs_get_float(m_storageHandle, "next_distance", &nextDistance);

  return nextDistance;
}

void Configuration::setLubricate(bool lubricate) {
  auto const value = static_cast<std::uint8_t>(lubricate);

  ESP_ERROR_CHECK(nvs_set_u8(m_storageHandle, "lubricate", value));
}

void Configuration::setManualLubricate(bool lubricate) {
  m_isManualLubricate = lubricate;
}

void Configuration::setPumpTimeout(std::uint32_t timeout) {
  ESP_ERROR_CHECK(nvs_set_u32(m_storageHandle, "pump_timeout", timeout));
}

void Configuration::setWheelLength(float wheelLength) {
  ESP_ERROR_CHECK(nvs_set_float(m_storageHandle, "wheel_length", wheelLength));
}

void Configuration::setMinimalSpeed(float minimalSpeed) {
  ESP_ERROR_CHECK(nvs_set_float(m_storageHandle, "minimal_speed", minimalSpeed));
}

void Configuration::setDistanceForEnable(float distance) {
  ESP_ERROR_CHECK(nvs_set_float(m_storageHandle, "distance_for_enable", distance));
}

void Configuration::saveTotalDistance(float distance) {
  ESP_ERROR_CHECK(nvs_set_float(m_storageHandle, "total_distance", distance));
}

void Configuration::saveNextDistance(float distance) {
  ESP_ERROR_CHECK(nvs_set_float(m_storageHandle, "next_distance", distance));
}
