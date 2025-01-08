//
// Created by jadjer on 10/2/24.
//

#include "Updater.hpp"

#include "esp_log.h"

auto constexpr TAG = "Update";

esp_partition_t const *getPartition() {
  auto const bootPartition = esp_ota_get_boot_partition();
  if (bootPartition == nullptr) {
    ESP_LOGE(TAG, "boot partition is NULL!");

    return nullptr;
  }

  if (bootPartition->type != ESP_PARTITION_TYPE_APP) {
    ESP_LOGE(TAG, "esp_current_partition->type != ESP_PARTITION_TYPE_APP");

    return nullptr;
  }

  auto partitionSubtype = ESP_PARTITION_SUBTYPE_APP_OTA_0;

  auto const nextPartition = esp_ota_get_next_update_partition(bootPartition);
  if (nextPartition) {
    partitionSubtype = nextPartition->subtype;
  }

  auto const partition = esp_partition_find_first(ESP_PARTITION_TYPE_APP, partitionSubtype, nullptr);
  if (partition == nullptr) {
    ESP_LOGE(TAG, "partition nullptr!");
    return nullptr;
  }

  return partition;
}

Updater::Updater() : m_partition(getPartition()) {
  esp_ota_mark_app_valid_cancel_rollback();
}

bool Updater::isStarted() const {
  if (m_partition == nullptr) {
    return false;
  }

  if (m_handle == 0) {
    return false;
  }

  return true;
}

bool Updater::begin(std::size_t const otaLength) {
  if (isStarted()) {
    if (esp_ota_abort(m_handle) != ESP_OK) {
      ESP_LOGE(TAG, "esp_ota_abort failed!");
    }
  }

  if (esp_ota_begin(m_partition, otaLength, &m_handle) != ESP_OK) {
    ESP_LOGE(TAG, "esp_ota_begin failed!");
    return false;
  }

  return true;
}

bool Updater::writeData(std::uint8_t const *data, std::size_t const dataLength) {
  if (not isStarted()) {
    ESP_LOGE(TAG, "Not started");
    return false;
  }

  if (esp_ota_write(m_handle, data, dataLength) != ESP_OK) {
    ESP_LOGE(TAG, "esp_ota_write failed!");

    if (esp_ota_abort(m_handle) != ESP_OK) {
      ESP_LOGE(TAG, "esp_ota_abort failed!");
    }

    m_handle = 0;

    return false;
  }

  return true;
}

bool Updater::end() {
  if (not isStarted()) {
    ESP_LOGE(TAG, "Not started");
    return false;
  }

  if (esp_ota_end(m_handle) != ESP_OK) {
    ESP_LOGE(TAG, "esp_ota_end failed!");

    return false;
  }

  if (esp_ota_set_boot_partition(m_partition) != ESP_OK) {
    ESP_LOGE(TAG, "esp_ota_set_boot_partition failed!");
    return false;
  }

  return true;
}
