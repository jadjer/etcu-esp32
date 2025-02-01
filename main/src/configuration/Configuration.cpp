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

#include "Configuration.hpp"

#include <cstring>
#include <nvs.h>
#include <nvs_flash.h>

#include "sdkconfig.h"

esp_err_t nvs_set_float(nvs_handle_t handle, char const *key, float value) {
  std::uint32_t buf = 0;

  std::memcpy(&buf, &value, sizeof(float));

  return nvs_set_u32(handle, key, buf);
}

esp_err_t nvs_get_float(nvs_handle_t handle, char const *key, float *value) {
  std::uint32_t buf = 0;

  esp_err_t err = nvs_get_u32(handle, key, &buf);
  if (err == ESP_OK) {
    std::memcpy(value, &buf, sizeof(float));
  }

  return err;
}

Configuration::Configuration() : m_storageHandle(0) {

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

interface::Pin Configuration::getIndicatorPin() const {
  return CONFIG_INDICATOR_PIN;
}
interface::Pin Configuration::getModeButtonPin() const {
  return CONFIG_MODE_SWITCH_PIN;
}
interface::Pin Configuration::getBreakSwitchPin() const {
  return CONFIG_BREAK_SWITCH_PIN;
}
interface::Pin Configuration::getGuardSwitchPin() const {
  return CONFIG_GUARD_SWITCH_PIN;
}
interface::Pin Configuration::getClutchSwitchPin() const {
  return CONFIG_CLUTCH_SWITCH_PIN;
}
interface::Pin Configuration::getTwistSensor1Pin() const {
  return CONFIG_TWIST_SENSOR_1_PIN;
}
interface::Pin Configuration::getTwistSensor2Pin() const {
  return CONFIG_TWIST_SENSOR_2_PIN;
}
