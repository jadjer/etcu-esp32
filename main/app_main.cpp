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

#include "nimble/Device.hpp"

void app_main() {
  nimble::Device::init("ETCU");

  auto const server = nimble::Device::createServer();

  auto const deviceInfoService = server->createService(nimble::UUID(static_cast<uint16_t>(0x180A)));
  auto const deviceName = deviceInfoService->createCharacteristic(nimble::UUID(static_cast<uint16_t>(0x2A00)), nimble::Property::READ);
  auto const deviceSerialNumber = deviceInfoService->createCharacteristic(nimble::UUID(static_cast<uint16_t>(0x2A25)), nimble::Property::READ);
  auto const deviceModelNumber = deviceInfoService->createCharacteristic(nimble::UUID(static_cast<uint16_t>(0x2A24)), nimble::Property::READ);

  deviceInfoService->start();

  server->start();

  auto const advertising = server->getAdvertising();
  advertising->addServiceUUID(deviceInfoService->getUUID());
  advertising->start();
}