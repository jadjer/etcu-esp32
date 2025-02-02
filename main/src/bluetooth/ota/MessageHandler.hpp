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

#include "NimBLECharacteristic.h"
#include "Updater.hpp"

enum Command {
  COMMAND_START = 0x0001,
  COMMAND_END = 0x0002,
  COMMAND_ACK = 0x0003,
};

enum CommandAck {
  COMMAND_ACK_ACCEPT = 0x0000,
  COMMAND_ACK_REFUSE = 0x0001,
};

enum BinaryAck {
  BINARY_ACK_SUCCESS = 0x0000,
  BINARY_ACK_CRC_ERROR = 0x0001,
  BINARY_ACK_SECTOR_INDEX_ERROR = 0x0002,
  BINARY_ACK_PAYLOAD_LENGTH_ERROR = 0x0003,
};

class MessageHandler {
  using Buffer = std::uint8_t *;
  using CharacteristicPtr = NimBLECharacteristic *;

public:
  MessageHandler(CharacteristicPtr dataCharacteristic, CharacteristicPtr commandCharacteristic);

public:
  void dataHandle(std::uint8_t const *data, std::size_t dataLength);
  void commandHandle(std::uint8_t const *data, std::size_t dataLength);

private:
  void resetBuffer();

private:
  UpdaterPtr m_updater = nullptr;
  CharacteristicPtr m_dataCharacteristic = nullptr;
  CharacteristicPtr m_commandCharacteristic = nullptr;

private:
  Buffer m_firmwareBuffer = nullptr;
  std::uint32_t m_firmwareBufferLength = 0;

private:
  std::uint32_t m_currentSector = 0;
  std::uint32_t m_currentPacket = 0;
};

#include <memory>

using MessageHandlerPtr = std::unique_ptr<MessageHandler>;
