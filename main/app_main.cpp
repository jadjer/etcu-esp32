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

#include <cstring>
#include <services/gap/ble_svc_gap.h>

#include "freertos/FreeRTOS.h"
#include "freertos/ringbuf.h"

#include "ble_ota.h"
#include "esp_bt.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "Updater.hpp"
#include "Bluetooth.hpp"
#include "Configuration.hpp"

static const char *TAG = "ETCU";

static Updater *updater = nullptr;
static Bluetooth *bluetooth = nullptr;
static Configuration *configuration = nullptr;

extern "C" void app_main(void) {
  configuration = new Configuration();

  updater = new Updater();

  bluetooth = new Bluetooth("ETCU");
  bluetooth->setUpdater(updater);
  bluetooth->start();

  //  esp_err_t ret;
  //  esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
  //
  //  if (!ble_ota_ringbuf_init(OTA_RINGBUF_SIZE)) {
  //    ESP_LOGE(TAG, "%s init ringbuf fail", __func__);
  //    return;
  //  }
  //
  //  ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));
  //
  //  ret = esp_bt_controller_init(&bt_cfg);
  //  if (ret) {
  //    ESP_LOGE(TAG, "%s enable controller failed: %s\n", __func__, esp_err_to_name(ret));
  //    return;
  //  }
  //
  //  ret = esp_bt_controller_enable(ESP_BT_MODE_BLE);
  //  if (ret) {
  //    ESP_LOGE(TAG, "%s enable controller failed: %s\n", __func__, esp_err_to_name(ret));
  //    return;
  //  }
  //
  //  if (esp_ble_ota_host_init() != ESP_OK) {
  //    ESP_LOGE(TAG, "%s initialoze ble host fail: %s\n", __func__, esp_err_to_name(ret));
  //    return;
  //  }
  //
  //  if (ble_svc_gap_device_name_set("ETCU") != ESP_OK) {
  //    ESP_LOGI(TAG, "set device name fail");
  //  }
  //
  //  esp_ble_ota_recv_fw_data_callback(ota_recv_fw_cb);
  //
  //  ota_task_init();
}
