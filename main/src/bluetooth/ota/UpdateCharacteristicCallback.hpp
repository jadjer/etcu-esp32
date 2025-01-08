//
// Created by jadjer on 10/2/24.
//

#pragma once

#include "NimBLECharacteristic.h"

#include <cstdint>

#include "MessageHandler.hpp"

class UpdateCharacteristicCallback : public NimBLECharacteristicCallbacks {
public:
  explicit UpdateCharacteristicCallback(MessageHandlerPtr messageHandler);
  ~UpdateCharacteristicCallback() override = default;

public:
  void onRead(NimBLECharacteristic *pCharacteristic, NimBLEConnInfo &connInfo) override;
  void onWrite(NimBLECharacteristic *pCharacteristic, NimBLEConnInfo &connInfo) override;
  void onStatus(NimBLECharacteristic *pCharacteristic, int code) override;
  void onSubscribe(NimBLECharacteristic *pCharacteristic, NimBLEConnInfo &connInfo, std::uint16_t subValue) override;

private:
  MessageHandlerPtr m_messageHandler = nullptr;
};
