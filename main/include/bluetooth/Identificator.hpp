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

#include "NimBLEUUID.h"

auto const SERVICE_CONFIGURATION_UUID = NimBLEUUID("019258ba-4978-7701-b004-f2e510aa86a2");
auto const CHARACTERISTIC_PUMP_TIMEOUT_UUID = NimBLEUUID("019258ba-4978-7a6a-9ae0-b71690b75329");
auto const CHARACTERISTIC_WHEEL_DIAMETER_UUID = NimBLEUUID("019258ba-4978-7120-bd38-ec62e59753ad");
auto const CHARACTERISTIC_MINIMAL_SPEED_UUID = NimBLEUUID("019258ba-4978-7882-a920-6c02d7cb09b9");
auto const CHARACTERISTIC_DISTANCE_FOR_ENABLE_UUID = NimBLEUUID("019258ba-4978-72af-ad9a-0e95d3238e90");
auto const CHARACTERISTIC_TOTAL_DISTANCE_UUID = NimBLEUUID("019258ba-4978-7d9a-976e-427a1c3037b7");
auto const CHARACTERISTIC_NEXT_DISTANCE_UUID = NimBLEUUID("019258ba-4978-7377-9443-6413a2f6472e");

auto const SERVICE_CONTROL_UUID = NimBLEUUID("019258ba-fe4d-7c5b-bfa2-4fb1e0106c40");
auto const CHARACTERISTIC_MANUAL_LUBRICATE_UUID = NimBLEUUID("019258ba-fe4d-7f31-97b8-3cbf2ff107d9");

auto const SERVICE_OTA_UUID = NimBLEUUID("0192590a-dd87-7dd0-8cce-73267971670b");
auto const CHARACTERISTIC_DATA_UUID = NimBLEUUID("0192590a-dd87-7f7b-9a30-9c0f50ae5bc1");
auto const CHARACTERISTIC_COMMAND_UUID = NimBLEUUID("0192590a-dd87-7c6f-bac9-1c9229e1211a");

auto const ADVERTISING_UUID = NimBLEUUID("01925900-cbcc-73ab-91e8-384d5f2b7767");
