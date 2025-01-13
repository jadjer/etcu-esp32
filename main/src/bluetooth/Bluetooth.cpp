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
// Created by jadjer on 9/25/24.
//

#include "bluetooth/Bluetooth.hpp"

#include "NimBLEDevice.h"

#include "ConfigurationCharacteristicCallback.hpp"
#include "bluetooth/Identificator.hpp"
#include "ota/MessageHandler.hpp"
#include "ota/UpdateCharacteristicCallback.hpp"

auto constexpr MTU = BLE_ATT_MTU_MAX;

Bluetooth::Bluetooth(ConfigurationPtr configuration) : m_otaCharacteristicCallback(nullptr),
                                                       m_configurationCharacteristicCallback(std::make_unique<ConfigurationCharacteristicCallback>(std::move(configuration))) {

  NimBLEDevice::init("CLS");
  NimBLEDevice::setMTU(MTU);

  auto const server = NimBLEDevice::createServer();
  server->advertiseOnDisconnect(true);

  {
    auto const service = server->createService(SERVICE_CONFIGURATION_UUID);

    auto const pumpTimeoutCharacteristic = service->createCharacteristic(CHARACTERISTIC_PUMP_TIMEOUT_UUID, NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE);
    auto const minimalSpeedCharacteristic = service->createCharacteristic(CHARACTERISTIC_MINIMAL_SPEED_UUID, NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE);
    auto const distanceForEnableCharacteristic = service->createCharacteristic(CHARACTERISTIC_DISTANCE_FOR_ENABLE_UUID, NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE);
    auto const totalDistanceCharacteristic = service->createCharacteristic(CHARACTERISTIC_TOTAL_DISTANCE_UUID, NIMBLE_PROPERTY::READ);
    auto const nextDistanceCharacteristic = service->createCharacteristic(CHARACTERISTIC_NEXT_DISTANCE_UUID, NIMBLE_PROPERTY::READ);

    pumpTimeoutCharacteristic->setCallbacks(m_configurationCharacteristicCallback.get());
    minimalSpeedCharacteristic->setCallbacks(m_configurationCharacteristicCallback.get());
    distanceForEnableCharacteristic->setCallbacks(m_configurationCharacteristicCallback.get());
    totalDistanceCharacteristic->setCallbacks(m_configurationCharacteristicCallback.get());
    nextDistanceCharacteristic->setCallbacks(m_configurationCharacteristicCallback.get());

    service->start();
  }

  {
    auto const service = server->createService(SERVICE_CONTROL_UUID);

    auto const manualLubricateCharacteristic = service->createCharacteristic(CHARACTERISTIC_MANUAL_LUBRICATE_UUID, NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE);

    manualLubricateCharacteristic->setCallbacks(m_configurationCharacteristicCallback.get());

    service->start();
  }

  {
    auto const service = server->createService(SERVICE_OTA_UUID);

    auto const dataCharacteristic = service->createCharacteristic(CHARACTERISTIC_DATA_UUID, NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::INDICATE);
    auto const commandCharacteristic = service->createCharacteristic(CHARACTERISTIC_COMMAND_UUID, NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::INDICATE);

    m_otaCharacteristicCallback = std::make_unique<UpdateCharacteristicCallback>(std::make_unique<MessageHandler>(dataCharacteristic, commandCharacteristic));

    dataCharacteristic->setCallbacks(m_otaCharacteristicCallback.get());
    commandCharacteristic->setCallbacks(m_otaCharacteristicCallback.get());

    service->start();
  }

  server->start();
}

Bluetooth::~Bluetooth() {
  NimBLEDevice::deinit();
}

void Bluetooth::advertise() {
  auto const advertising = NimBLEDevice::getAdvertising();
  advertising->setManufacturerData("jadjer");
  advertising->addServiceUUID(ADVERTISING_UUID);
  advertising->start();
}
