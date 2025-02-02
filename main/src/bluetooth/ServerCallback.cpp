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

#include "ServerCallback.hpp"

auto const TAG = "Server Callback";

void ServerCallback::onConnect(NimBLEServer *pServer, NimBLEConnInfo &connInfo) {
  NimBLEServerCallbacks::onConnect(pServer, connInfo);

  ESP_LOGI(TAG, "onConnect");
}

void ServerCallback::onDisconnect(NimBLEServer *pServer, NimBLEConnInfo &connInfo, int reason) {
  NimBLEServerCallbacks::onDisconnect(pServer, connInfo, reason);

  ESP_LOGI(TAG, "onDisconnect");
}

void ServerCallback::onMTUChange(uint16_t MTU, NimBLEConnInfo &connInfo) {
  NimBLEServerCallbacks::onMTUChange(MTU, connInfo);

  ESP_LOGI(TAG, "onMTUChange");
}

uint32_t ServerCallback::onPassKeyDisplay() {
  return NimBLEServerCallbacks::onPassKeyDisplay();
}

void ServerCallback::onConfirmPassKey(NimBLEConnInfo &connInfo, uint32_t pin) {
  NimBLEServerCallbacks::onConfirmPassKey(connInfo, pin);

  ESP_LOGI(TAG, "onConfirmPassKey");
}

void ServerCallback::onAuthenticationComplete(NimBLEConnInfo &connInfo) {
  NimBLEServerCallbacks::onAuthenticationComplete(connInfo);

  ESP_LOGI(TAG, "onAuthenticationComplete");
}

void ServerCallback::onIdentity(NimBLEConnInfo &connInfo) {
  NimBLEServerCallbacks::onIdentity(connInfo);

  ESP_LOGI(TAG, "onIdentity");
}

void ServerCallback::onConnParamsUpdate(NimBLEConnInfo &connInfo) {
  NimBLEServerCallbacks::onConnParamsUpdate(connInfo);

  ESP_LOGI(TAG, "onConnParamsUpdate");
}
