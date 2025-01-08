//
// Created by jadjer on 10/2/24.
//

#pragma once

#include <cstdint>
#include <esp_ota_ops.h>
#include <string>

class Updater {
  using PartitionPtr = esp_partition_t const *;

public:
  Updater();

public:
  [[nodiscard]] bool isStarted() const;

public:
  bool begin(std::size_t otaLength);
  bool writeData(std::uint8_t const *data, std::size_t dataLength);
  bool end();

private:
  PartitionPtr m_partition = nullptr;
  esp_ota_handle_t m_handle = 0;
};

#include <memory>

using UpdaterPtr = std::unique_ptr<Updater>;
