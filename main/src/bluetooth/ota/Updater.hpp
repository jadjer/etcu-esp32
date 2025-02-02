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
