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

#include "UpdateCharacteristicCallback.hpp"

#include "bluetooth/Identificator.hpp"

UpdateCharacteristicCallback::UpdateCharacteristicCallback(MessageHandlerPtr messageHandler) : m_messageHandler(std::move(messageHandler)) {
}

void UpdateCharacteristicCallback::onRead(NimBLECharacteristic *pCharacteristic, NimBLEConnInfo &connInfo) {
  NimBLECharacteristicCallbacks::onRead(pCharacteristic, connInfo);
}

void UpdateCharacteristicCallback::onWrite(NimBLECharacteristic *pCharacteristic, NimBLEConnInfo &connInfo) {
  NimBLECharacteristicCallbacks::onWrite(pCharacteristic, connInfo);

  NimBLEUUID const uuid = pCharacteristic->getUUID();

  if (uuid == CHARACTERISTIC_DATA_UUID) {
    auto const value = pCharacteristic->getValue();

    auto const data = value.data();
    auto const length = value.length();

    m_messageHandler->dataHandle(data, length);
  }

  if (uuid == CHARACTERISTIC_COMMAND_UUID) {
    auto const value = pCharacteristic->getValue();

    auto const data = value.data();
    auto const length = value.length();

    m_messageHandler->commandHandle(data, length);
  }
}

void UpdateCharacteristicCallback::onStatus(NimBLECharacteristic *pCharacteristic, int const code) {
  NimBLECharacteristicCallbacks::onStatus(pCharacteristic, code);
}

void UpdateCharacteristicCallback::onSubscribe(NimBLECharacteristic *pCharacteristic, NimBLEConnInfo &connInfo, std::uint16_t const subValue) {
  NimBLECharacteristicCallbacks::onSubscribe(pCharacteristic, connInfo, subValue);
}
