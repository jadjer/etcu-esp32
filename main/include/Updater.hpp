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

#pragma once

#include <esp_ota_ops.h>
#include <esp_partition.h>
#include <freertos/ringbuf.h>

class Updater {
public:
  Updater();
  ~Updater();

public:
  size_t writeData(void const *data, size_t dataSize);
  esp_err_t finish() const;
  esp_err_t abort() const;

private:
  static void otaTask(void *arg);

private:
  esp_err_t writeImagePart(void const *data, size_t dataSize);

private:
  RingbufHandle_t m_ringBuffer = nullptr;
  esp_partition_t const *m_otaPartition = nullptr;

private:
  bool m_imageHeaderWasChecked = false;
  esp_ota_handle_t m_otaHandle = 0;
};
