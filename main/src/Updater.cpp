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

#include "Updater.hpp"
#include "ble_ota.h"

#include <esp_log.h>
#include <esp_ota_ops.h>
#include <esp_system.h>
#include <freertos/ringbuf.h>

constexpr size_t OTA_RINGBUF_SIZE = 8192;
constexpr size_t OTA_TASK_SIZE = 8192;

constexpr char const *TAG = "OTA_UPDATER";

Updater::Updater() : m_ringBuffer(xRingbufferCreate(OTA_RINGBUF_SIZE, RINGBUF_TYPE_BYTEBUF)),
                     m_otaPartition(esp_ota_get_next_update_partition(nullptr)) {
  assert(m_ringBuffer);
  assert(m_otaPartition);

  xTaskCreate(&Updater::otaTask, "ota_task", OTA_TASK_SIZE, this, 5, nullptr);
}

Updater::~Updater() {
  abort();
}

size_t Updater::writeData(void const *data, size_t const dataSize) {
  BaseType_t done = xRingbufferSend(m_ringBuffer, data, dataSize, portMAX_DELAY);
  if (done) {
    return dataSize;
  } else {
    return 0;
  }
}

esp_err_t Updater::finish() const {
  auto errorCode = esp_ota_end(m_otaHandle);
  if (errorCode != ESP_OK) {
    ESP_LOGE(TAG, "esp_ota_end failed");
    return errorCode;
  }

  errorCode = esp_ota_set_boot_partition(m_otaPartition);
  if (errorCode != ESP_OK) {
    ESP_LOGE(TAG, "esp_ota_set_boot_partition failed!\r\n");
    return errorCode;
  }

  esp_restart();
}

esp_err_t Updater::abort() const {
  auto const errorCode = esp_ota_abort(m_otaHandle);
  if (errorCode != ESP_OK) {
    ESP_LOGE(TAG, "esp_ota_abort failed");
    return errorCode;
  }

  return ESP_OK;
}

void Updater::otaTask(void *arg) {
  auto updater = static_cast<Updater *>(arg);

  ESP_LOGI(TAG, "ota_task start");

  size_t partSize = 0;
  uint32_t received = 0;

  ESP_LOGI(TAG, "wait for data from ringbuf! fw_len = %u", esp_ble_ota_get_fw_length());

  for (;;) {
    auto const imagePart = xRingbufferReceive(updater->m_ringBuffer, &partSize, portMAX_DELAY);

    ESP_LOGI(TAG, "recv: %u, recv_total:%" PRIu32 "\n", partSize, received + partSize);

    if (partSize != 0) {
      if (updater->writeData(imagePart, partSize) != ESP_OK) {
        ESP_LOGE(TAG, "esp_ota_write failed!\r\n");
      }

      received += partSize;

      vRingbufferReturnItem(updater->m_ringBuffer, imagePart);

      if (received >= esp_ble_ota_get_fw_length()) {
        ESP_LOGI(TAG, "Received all data");
        break;
      }
    }
  }

  if (updater->finish() != ESP_OK) {
    ESP_LOGE(TAG, "esp_ota_end failed");
  }

  ESP_LOGI(TAG, "finish ota");
}

esp_err_t Updater::writeImagePart(const void *data, size_t dataSize) {
  if (m_otaPartition == nullptr) {
    return ESP_ERR_INVALID_STATE;
  }

  if (dataSize == 0) {
    return ESP_ERR_INVALID_SIZE;
  }

  if (not m_imageHeaderWasChecked) {
    //    esp_app_desc_t new_app_info;
    //
    //    auto const headerSize = sizeof(esp_image_header_t) + sizeof(esp_image_segment_header_t) + sizeof(esp_app_desc_t);
    //    if (headerSize < dataSize) {
    //      // check current version with downloading
    //      if (not esp_efuse_check_secure_version(new_app_info.secure_version)) {
    //        ESP_LOGE(TAG, "This a new app can not be downloaded due to a secure version is lower than stored in efuse.");
    //        return;
    //      }
    //    }

    m_imageHeaderWasChecked = true;

    auto errorCode = esp_ota_begin(m_otaPartition, OTA_SIZE_UNKNOWN, &m_otaHandle);
    if (errorCode != ESP_OK) {
      ESP_LOGE(TAG, "esp_ota_begin failed");
      return errorCode;
    }
  }

  auto errorCode = esp_ota_write(m_otaHandle, data, dataSize);
  if (errorCode != ESP_OK) {
    ESP_LOGE(TAG, "esp_ota_write failed");
    return errorCode;
  }

  return ESP_OK;
}
