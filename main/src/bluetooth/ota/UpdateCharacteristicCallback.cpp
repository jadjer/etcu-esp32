//
// Created by jadjer on 10/2/24.
//

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
