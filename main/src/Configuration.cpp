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
// Created by jadjer on 5/16/24.
//

#include "Configuration.hpp"

#include <esp_log.h>
#include <nvs_flash.h>

constexpr char const * TAG = "Configuration";

Configuration::Configuration() {
  auto returnCode = nvs_flash_init();
  if (returnCode == ESP_ERR_NVS_NO_FREE_PAGES or returnCode == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    returnCode = nvs_flash_init();
  }
  ESP_ERROR_CHECK(returnCode);

  returnCode = nvs_open("ETCU", NVS_READWRITE, &m_handle);
  ESP_ERROR_CHECK(returnCode);
}

Configuration::~Configuration() {
  nvs_close(m_handle);
  nvs_flash_deinit();
}

uint32_t Configuration::getValue(const char *name) const {
  uint32_t value = 0;

  auto const returnCode = nvs_get_u32(m_handle, name, &value);
  if (returnCode == ESP_ERR_NVS_NOT_FOUND) {
    return 0;
  }
  ESP_ERROR_CHECK(returnCode);

  return value;
}

void Configuration::setValue(const char *name, uint32_t value) const {
  auto errorCode = nvs_set_u32(m_handle, name, value);
  ESP_ERROR_CHECK(errorCode);

  errorCode = nvs_commit(m_handle);
  ESP_ERROR_CHECK(errorCode);
}
